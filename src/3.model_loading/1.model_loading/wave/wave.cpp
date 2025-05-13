#include "wave/wave.h"
#include <time.h> 

// GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define RECORD 0
#include <fftw/fftw3.h>

// Get k vector from mesh grid (n,m)
#define K_VEC(n,m) vec2(2 * M_PI * (n - N / 2) / L_x, 2 * M_PI * (m  - M / 2) / L_z)

#if RECORD	// Record

int record = 1;
#define MESH_RESOLUTION 1024

#else

int record = 0;
#define MESH_RESOLUTION 64

#endif

// Mesh resolution
int N = MESH_RESOLUTION;
int M = MESH_RESOLUTION;
float L_x = 1000;
float L_z = 1000;

float A = 1.25e-8f;
// Wind speed
float V = 30;
// Wind direction
vec2 omega(1, 1);

// Light attributes
glm::vec3 lampPos(0.0f, 10.0f, 0.0f);
glm::vec3 sundir(normalize(vec3(0, 1, -2)));
glm::vec3 suncol(1.0f, 1.0f, 1.0f);
glm::vec3 seacolor(0.0f, 0.0, 0.5);

glm::vec3* vertexArray;
GLuint surfaceVAO = 0;
GLuint surfaceVBO, EBO,lamp_EBO;
GLuint VBO;
GLuint lightVAO;
int indexSize;

float time_ = 0;
float modelScale = 0.1f;

int nWaves = 60;
glm::vec4* waveParams;

float height;

float heightMax = 0;
float heightMin = 0;
float offset_y = 14.2f;
const int slice = 360;
const int stack = 180;
std::vector<GLfloat> vertices;
std::vector<GLuint> indices_lamp;

Wave::Wave(int N, int M, float L_x, float L_z, vec2 omega, float V, float A, float lambda) :
	N(N), M(M),
	omega_hat(normalize(omega)),
	V(V), L_x(L_x), L_z(L_z),
	A(A),
	lambda(lambda)
{
	generator.seed(time(NULL));
	kNum = N * M;

	heightField = new glm::vec3[kNum];
	normalField = new glm::vec3[kNum];

	value_h_twiddle_0 = new complex<float>[kNum];
	value_h_twiddle_0_conj = new complex<float>[kNum];
	value_h_twiddle = new complex<float>[kNum];

	// Initialize value_h_twiddle_0 and value_h_twiddle_0_conj in Eq26
	for (int n = 0; n < N; ++n)
		for (int m = 0; m < M; ++m)
		{
			int index = m * N + n;
			vec2 k = K_VEC(n, m);
			value_h_twiddle_0[index] = func_h_twiddle_0(k);
			value_h_twiddle_0_conj[index] = conj(func_h_twiddle_0(k));
		}		
}

Wave::~Wave()
{
	delete[] heightField;
	delete[] normalField;

	delete[] value_h_twiddle_0;
	delete[] value_h_twiddle;
	delete[] value_h_twiddle_0_conj;
}

// Eq14
inline float Wave::func_omega(float k) const
{
	return sqrt(g*k);
}

// Eq23 Phillips spectrum 
inline float Wave::func_P_h(vec2 vec_k) const
{
	if (vec_k == vec2(0.0f, 0.0f))
		return 0.0f;

	float L = V*V / g; // Largest possible waves arising from a continuous wind of speed V

	float k = length(vec_k);
	vec2 k_hat = normalize(vec_k);

	float dot_k_hat_omega_hat = dot(k_hat, omega_hat);
	float result = A * exp(-1 / (k*L*k*L)) / pow(k, 4) * pow(dot_k_hat_omega_hat, 2);

	result *= exp(-k*k*l*l);  // Eq24

	return result;
}

// Eq25
inline complex<float> Wave::func_h_twiddle_0(vec2 vec_k)
{
	float xi_r = normal_dist(generator);
	float xi_i = normal_dist(generator);
	return sqrt(0.5f) * complex<float>(xi_r, xi_i) * sqrt(func_P_h(vec_k));
}

// Eq26
inline complex<float> Wave::func_h_twiddle(int kn, int km, float t) const
{
	int index = km * N + kn;
	float k = length(K_VEC(kn, km));
	complex<float> term1 = value_h_twiddle_0[index] * exp(complex<float>(0.0f, func_omega(k)*t));
	complex<float> term2 = value_h_twiddle_0_conj[index] * exp(complex<float>(0.0f, -func_omega(k)*t));
	return term1 + term2;
}

//Eq19
void Wave::buildField(float time_)
{
	fftwf_complex *in_height, *in_slope_x, *in_slope_z, *in_D_x, *in_D_z;
	fftwf_complex *out_height, *out_slope_x, *out_slope_z, *out_D_x, *out_D_z;

	fftwf_plan p_height, p_slope_x, p_slope_z, p_D_x, p_D_z;

	// Eq20 ikh_twiddle
	complex<float>* slope_x_term = new complex<float>[kNum];
	complex<float>* slope_z_term = new complex<float>[kNum];

	// Eq29 
	complex<float>* D_x_term = new complex<float>[kNum];
	complex<float>* D_z_term = new complex<float>[kNum];

	for (int n = 0; n < N; n++)
		for (int m = 0; m < M; m++)
		{
			int index = m * N + n;			

			value_h_twiddle[index] = func_h_twiddle(n, m, time_);

			vec2 kVec = K_VEC(n, m);
			float kLength = length(kVec);
			vec2 kVecNormalized = kLength == 0 ? kVec : normalize(kVec);

			slope_x_term[index] = complex<float>(0, kVec.x) * value_h_twiddle[index];
			slope_z_term[index] = complex<float>(0, kVec.y) * value_h_twiddle[index];
			D_x_term[index] = complex<float>(0, -kVecNormalized.x) * value_h_twiddle[index];
			D_z_term[index] = complex<float>(0, -kVecNormalized.y) * value_h_twiddle[index];
		}

	// Prepare fft input and output
	in_height = (fftwf_complex*)value_h_twiddle;
	in_slope_x = (fftwf_complex*)slope_x_term;
	in_slope_z = (fftwf_complex*)slope_z_term;
	in_D_x = (fftwf_complex*)D_x_term;
	in_D_z = (fftwf_complex*)D_z_term;

	out_height = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * kNum);
	out_slope_x = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * kNum);
	out_slope_z = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * kNum);
	out_D_x = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * kNum);
	out_D_z = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * kNum);

	p_height = fftwf_plan_dft_2d(N, M, in_height, out_height, FFTW_BACKWARD, FFTW_ESTIMATE);
	p_slope_x = fftwf_plan_dft_2d(N, M, in_slope_x, out_slope_x, FFTW_BACKWARD, FFTW_ESTIMATE);
	p_slope_z = fftwf_plan_dft_2d(N, M, in_slope_z, out_slope_z, FFTW_BACKWARD, FFTW_ESTIMATE);
	p_D_x = fftwf_plan_dft_2d(N, M, in_D_x, out_D_x, FFTW_BACKWARD, FFTW_ESTIMATE);
	p_D_z = fftwf_plan_dft_2d(N, M, in_D_z, out_D_z, FFTW_BACKWARD, FFTW_ESTIMATE);

	fftwf_execute(p_height);
	fftwf_execute(p_slope_x);
	fftwf_execute(p_slope_z);
	fftwf_execute(p_D_x);
	fftwf_execute(p_D_z);

	for (int n = 0; n < N; n++)
		for (int m = 0; m < M; m++)
		{
			int index = m * N + n;
			float sign = 1;

			// Flip the sign
			if ((m + n) % 2) sign = -1;
			
			normalField[index] = normalize(vec3(
				sign * out_slope_x[index][0], 
				-1,
				sign * out_slope_z[index][0]));

			heightField[index] = vec3(
				(n - N / 2) * L_x / N - sign * lambda * out_D_x[index][0],
				sign * out_height[index][0] + offset_y,
				(m - M / 2) * L_z / M - sign * lambda * out_D_z[index][0]);
		}

	fftwf_destroy_plan(p_height);
	fftwf_destroy_plan(p_slope_x);
	fftwf_destroy_plan(p_slope_z);
	fftwf_destroy_plan(p_D_x);
	fftwf_destroy_plan(p_D_z);


	// Free
	delete[] slope_x_term;
	delete[] slope_z_term;
	delete[] D_x_term;
	delete[] D_z_term;
	fftwf_free(out_height);
	fftwf_free(out_slope_x);
	fftwf_free(out_slope_z);
	fftwf_free(out_D_x);
	fftwf_free(out_D_z);
}

void initBufferObjects()
{
	indexSize = (N - 1) * (M - 1) * 6;
	GLuint* indices = new GLuint[indexSize];

	int p = 0;

	for (int j = 0; j < N - 1; j++)
		for (int i = 0; i < M - 1; i++)
		{
			indices[p++] = i + j * N;
			indices[p++] = (i + 1) + j * N;
			indices[p++] = i + (j + 1) * N;

			indices[p++] = (i + 1) + j * N;
			indices[p++] = (i + 1) + (j + 1) * N;
			indices[p++] = i + (j + 1) * N;
		}

	// Element buffer object
	glGenVertexArrays(1, &surfaceVAO);
	glBindVertexArray(surfaceVAO);
	glGenBuffers(1, &surfaceVBO);
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * sizeof(GLuint), indices, GL_STATIC_DRAW);

	delete[] indices;
}

void deleteBufferObjects()
{
	glDeleteVertexArrays(1, &surfaceVAO);
	glDeleteBuffers(1, &surfaceVBO);
	glDeleteBuffers(1, &EBO);
}

// _WAVE_
// Build the mesh using the height provided by the algorithm.
void buildTessendorfWaveMesh(Wave* wave_model)
{
	int nVertex = N * M;

	wave_model->buildField(time_);
	vec3* heightField = wave_model->heightField;
	vec3* normalField = wave_model->normalField;

	int p = 0;

	for (int i = 0; i < N; i++)
		for (int j = 0; j < M; j++)
		{
			int index = j * N + i;

			if (heightField[index].y > heightMax) heightMax = heightField[index].y;
			else if (heightField[index].y < heightMin) heightMin = heightField[index].y;
		}


	glBindVertexArray(surfaceVAO);
	glBindBuffer(GL_ARRAY_BUFFER, surfaceVBO);

	int fieldArraySize = sizeof(glm::vec3) * nVertex;
	glBufferData(GL_ARRAY_BUFFER, fieldArraySize * 2, NULL, GL_STATIC_DRAW);

	// Copy height to buffer
	glEnableVertexAttribArray(0);
	glBufferSubData(GL_ARRAY_BUFFER, 0, fieldArraySize, heightField);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	

	// Copy normal to buffer
	glEnableVertexAttribArray(1);
	glBufferSubData(GL_ARRAY_BUFFER, fieldArraySize, fieldArraySize, normalField);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)fieldArraySize);
	
}
void sphere(){
	vertices.reserve(8 * (stack + 1) * (slice + 1));
	indices_lamp.reserve(6 * stack * slice);
	float x, y, z, xy;  //  position
	float s, t;         //  texCoord

	float sectorStep = glm::two_pi<float>() / slice;
	float stackStep = glm::pi<float>() / stack;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stack; ++i) {
		stackAngle = static_cast<float>(glm::half_pi<float>() - i * stackStep);  // [pi/2, -pi/2]
		xy = cosf(stackAngle);                                                   // r * cos(u)
		z = sinf(stackAngle);                                                    // r * sin(u)

		for (int j = 0; j <= slice; ++j) {
			sectorAngle = j * sectorStep;  // [0, 2pi]

			x = xy * cosf(sectorAngle);  // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle);  // r * cos(u) * sin(v)

			// vertex tex coord (s, t) range between [0, 1]
			s = static_cast<float>(j) / slice;
			t = static_cast<float>(i) / stack;
			vertices.insert(vertices.end(), { x, y, z, x, y, z, s, t });
		}
	}
	unsigned int k1, k2;  // EBO index
	for (int i = 0; i < stack; ++i) {
		k1 = i * (slice + 1);  // beginning of current stack
		k2 = k1 + slice + 1;   // beginning of next stack
		for (int j = 0; j < slice; ++j, ++k1, ++k2) {
			if (i != 0) {
				indices_lamp.insert(indices_lamp.end(), { k1, k2, k1 + 1 });
			}
			// k1+1 => k2 => k2+1
			if (i != (stack - 1)) {
				indices_lamp.insert(indices_lamp.end(), { k1 + 1, k2, k2 + 1 });
			}
		}
	}
}
// Light
Wave* InitWave() {
	lampPos = sundir * 50.0f;
	sphere();
	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0); // Note that we skip over the other data in our buffer object (we don't need the normals/textures, only positions).
	glGenBuffers(1, &lamp_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lamp_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_lamp.size() * sizeof(GLuint), indices_lamp.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
	initBufferObjects();
	// _WAVE_
	// Speed of wind, direction of wind
	time_ = 0;
	
	return new Wave(N, M, L_x, L_z, omega, V, A, 1);
}