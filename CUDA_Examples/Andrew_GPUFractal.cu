//nvcc GPUFractal.cu -o GPUFrac -lGLEW -lglut -lm -lGLU -lGL
#define GL_GLEXT_PROTOTYPES

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdlib.h>
#include <stdio.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define BUFFER_DATA(i) ((char *)0 + i)

unsigned int window_width = 1024;
unsigned int window_height = 1024;

float dt;

int frameCount = 0;
int currentTime = 0;
int previousTime = 0;

float *pixels;
float *pixels_gpu;

float ab[2] = {0,0};
float offset[2] = {0,0};
float scale = 2;
unsigned int loops = 500;

GLuint gl_PBO, gl_Tex, gl_Shader;
struct cudaGraphicsResource *cuda_pbo_resource;

uchar4 *h_Src = 0;
uchar4 *d_dst = NULL;

static const char *shader_code =
    "!!ARBfp1.0\n"
    "TEX result.color, fragment.texcoord, texture[0], 2D; \n"
    "END";

__device__ float ab_gpu[2];
__device__ float offset_gpu[2];
__device__ float scale_gpu;
__device__ unsigned int loops_gpu;

__device__ unsigned char fractal(){
	int i = 0;
	float a = ab_gpu[0];
	float b = ab_gpu[1];
	float scale = scale_gpu;
	float xOffset = offset_gpu[1];
	float yOffset = offset_gpu[0];
	float x,y;
	x = (((float)threadIdx.x / (float)blockDim.x) * (scale_gpu*2)) - scale + xOffset;
	y = (((float)blockIdx.x / (float)gridDim.x) * (scale_gpu*2)) - scale + yOffset;
	float dist = 10000000000000;
	while(i < loops_gpu){
		float tmpX = (x*x) - (y*y) + a;
		y = 2*x*y + b;
		x = tmpX;
		if(x*x + y*y > 4){
			i=0;
			break;
		}
		i++;
		dist = min(dist, x*x + y*y);
	}

	return (unsigned char)((float)i/((float)loops_gpu)*512*sqrt(dist));
}

__global__ void fractalSetPixels(uchar4 *pixels_gpu){
	int tid = (threadIdx.x) + blockIdx.x * (blockDim.x);

	int contrast = fractal();

	pixels_gpu[tid].x = 0;
	pixels_gpu[tid].y = contrast;
	pixels_gpu[tid].z = 0;
	pixels_gpu[tid].w = 0;
}

void computeFPS(){

	frameCount++;

	currentTime = glutGet(GLUT_ELAPSED_TIME);

	int timeInterval = currentTime - previousTime;

	if(timeInterval > 1000){
		char fps[256];

		sprintf(fps, "%3.1f fps", frameCount / (timeInterval / 1000.f));

		glutSetWindowTitle(fps);

		previousTime = currentTime;

		frameCount = 0;
	}
}

int oldTimeSinceStart = 0;
void display(void)
{
	int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	dt = (float)(timeSinceStart - oldTimeSinceStart)/1000.0f;
	oldTimeSinceStart = timeSinceStart;
	glFlush();

	cudaGraphicsMapResources(1, &cuda_pbo_resource, 0);
	size_t num_bytes;
	cudaGraphicsResourceGetMappedPointer((void **)&d_dst, &num_bytes, cuda_pbo_resource);

	fractalSetPixels<<<window_width, window_height>>>(d_dst);

	cudaDeviceSynchronize();

	cudaGraphicsUnmapResources(1, &cuda_pbo_resource, 0);

	glBindTexture(GL_TEXTURE_2D, gl_Tex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, window_width, window_height, GL_RGBA, GL_UNSIGNED_BYTE, BUFFER_DATA(0));

	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, gl_Shader);
	glEnable(GL_FRAGMENT_PROGRAM_ARB);
	glDisable(GL_DEPTH_TEST);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_FRAGMENT_PROGRAM_ARB);


	//cudaMemcpy(pixels, pixels_gpu, window_width*window_height*3*sizeof(float), cudaMemcpyDeviceToHost);
	//glDrawPixels(window_width, window_height, GL_RGB, GL_FLOAT, pixels_gpu);
	glutSwapBuffers();

	computeFPS();
}

void reset(){
	scale = 2;
	offset[0] = 0;
	offset[1] = 0;
	ab[0] = -0.747753;
	ab[1] = 0.05f;

	cudaMemcpyToSymbol(scale_gpu, &scale, sizeof(float), 0, cudaMemcpyHostToDevice);
	cudaMemcpyToSymbol(offset_gpu, offset, 2*sizeof(float), 0, cudaMemcpyHostToDevice);
	cudaMemcpyToSymbol(ab_gpu, ab, 2*sizeof(float), 0, cudaMemcpyHostToDevice);
}

void keyboard(unsigned char key, int x, int y){
	switch(key){
	case 'w':
		ab[0] += 0.1f * dt;
		cudaMemcpyToSymbol(ab_gpu, ab, 2*sizeof(float), 0, cudaMemcpyHostToDevice);
		break;
	case 's':
		ab[0] -= 0.1f * dt;
		cudaMemcpyToSymbol(ab_gpu, ab, 2*sizeof(float), 0, cudaMemcpyHostToDevice);
		break;
	case 'a':
		ab[1] -= 0.1f * dt;
		cudaMemcpyToSymbol(ab_gpu, ab, 2*sizeof(float), 0, cudaMemcpyHostToDevice);
		break;
	case 'd':
		ab[1] += 0.1f * dt;
		cudaMemcpyToSymbol(ab_gpu, ab, 2*sizeof(float), 0, cudaMemcpyHostToDevice);
		break;
	case 'q':
		scale -= 0.5f * scale * dt;
		cudaMemcpyToSymbol(scale_gpu, &scale, sizeof(float), 0, cudaMemcpyHostToDevice);
		break;
	case 'e':
		scale += 0.5f * scale * dt;
		cudaMemcpyToSymbol(scale_gpu, &scale, sizeof(float), 0, cudaMemcpyHostToDevice);
		break;
	case 'r':
		reset();
		break;
	case ' ':
		printf("===============================");
		printf("\na = %f, b = %f\n", ab[0], ab[1]);
		printf("scale = %f\n", scale);
		printf("offset X = %f \t offset Y = %f\n", offset[1], offset[0]);
		printf("max iterations = %d\n", loops);
		printf("===============================\n");
		break;
	}

	//glutPostRedisplay();
}

void specialInput(int key, int x, int Y){
	switch(key){
	case GLUT_KEY_UP:
		offset[0] += 0.1f * scale * dt;
		cudaMemcpyToSymbol(offset_gpu, offset, 2*sizeof(float), 0, cudaMemcpyHostToDevice);
		break;
	case GLUT_KEY_DOWN:
		offset[0] -= 0.1f * scale * dt;
		cudaMemcpyToSymbol(offset_gpu, offset, 2*sizeof(float), 0, cudaMemcpyHostToDevice);
		break;
	case GLUT_KEY_LEFT:
		offset[1] -= 0.1f * scale * dt;
		cudaMemcpyToSymbol(offset_gpu, offset, 2*sizeof(float), 0, cudaMemcpyHostToDevice);
		break;
	case GLUT_KEY_RIGHT:
		offset[1] += 0.1f * scale * dt;
		cudaMemcpyToSymbol(offset_gpu, offset, 2*sizeof(float), 0, cudaMemcpyHostToDevice);
		break;
	case GLUT_KEY_PAGE_UP:
		loops += 100;
		cudaMemcpyToSymbol(loops_gpu, &loops, sizeof(int), 0, cudaMemcpyHostToDevice);
		break;
	case GLUT_KEY_PAGE_DOWN:
		if(loops - 100 > 0){
			loops -= 100;
		}
		cudaMemcpyToSymbol(loops_gpu, &loops, sizeof(int), 0, cudaMemcpyHostToDevice);
		break;
	}
	//glutPostRedisplay();
}

GLuint compileASMShader(GLenum program_type, const char *code){
	GLuint program_id;
	glGenProgramsARB(1, &program_id);
	glBindProgramARB(program_type, program_id);
	glProgramStringARB(program_type, GL_PROGRAM_FORMAT_ASCII_ARB, (GLsizei) strlen(code), (GLubyte *) code);

	GLint error_pos;
	glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &error_pos);

	if(error_pos != -1){
		const GLubyte *error_string;
		error_string = glGetString(GL_PROGRAM_ERROR_STRING_ARB);
		fprintf(stderr, "Program error at position: %d\b%s\n", (int)error_pos, error_string);
		return 0;
	}

	return program_id;
}

void init(){

	if (h_Src)
	{
		free(h_Src);
		h_Src = 0;
	}

	if (gl_Tex)
	{
		glDeleteTextures(1, &gl_Tex);
		gl_Tex = 0;
	}

	if (gl_PBO)
	{
		cudaGraphicsUnregisterResource(cuda_pbo_resource);
		glDeleteBuffers(1, &gl_PBO);
		gl_PBO = 0;
	}

	h_Src = (uchar4 *)malloc(window_width * window_height * 4);

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &gl_Tex);
	glBindTexture(GL_TEXTURE_2D, gl_Tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, h_Src);

	glGenBuffers(1, &gl_PBO);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, gl_PBO);
	glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, window_width * window_height * sizeof(uchar4), NULL, GL_STREAM_COPY);

	cudaGraphicsGLRegisterBuffer(&cuda_pbo_resource, gl_PBO, cudaGraphicsMapFlagsWriteDiscard);

	gl_Shader = compileASMShader(GL_FRAGMENT_PROGRAM_ARB, shader_code);


	cudaGraphicsMapResources(1, &cuda_pbo_resource, 0);
	size_t num_bytes;
	cudaGraphicsResourceGetMappedPointer((void**)&d_dst, &num_bytes, cuda_pbo_resource);

	pixels = (float *)malloc(window_width*window_height*3*sizeof(float));
	cudaMalloc((void**)&pixels_gpu, window_width*window_height*3*sizeof(float));

	cudaMemcpyToSymbol(ab_gpu, ab, 2*sizeof(float), 0, cudaMemcpyHostToDevice);
	cudaMemcpyToSymbol(scale_gpu, &scale, sizeof(float), 0, cudaMemcpyHostToDevice);
	cudaMemcpyToSymbol(offset_gpu, offset, 2*sizeof(float), 0, cudaMemcpyHostToDevice);
	cudaMemcpyToSymbol(loops_gpu, &loops, sizeof(int), 0, cudaMemcpyHostToDevice);
}

void deInit(){

	if(h_Src){
		free(h_Src);
		h_Src = 0;
	}

	free(pixels);
	cudaFree(pixels_gpu);

	cudaGraphicsUnmapResources(1, &cuda_pbo_resource, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

	glDeleteBuffers(1, &gl_PBO);
	glDeleteTextures(1, &gl_Tex);
	glDeleteProgramsARB(1, &gl_Shader);
}

void idle(void){

	computeFPS();

	glutPostRedisplay();
}

int main(int argc, char** argv)
{
   	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
   	glutInitWindowSize(window_width, window_height);
   	glutCreateWindow("BitMap");
   	glutDisplayFunc(display);
   	glutKeyboardFunc(keyboard);
   	glutSpecialFunc(specialInput);
   	glutIdleFunc(idle);

   	printf("Loading extensions: %s\n", glewGetErrorString(glewInit()));

   	if (!glewIsSupported("GL_VERSION_1_5 GL_ARB_vertex_buffer_object GL_ARB_pixel_buffer_object"))
   	    {
   	        fprintf(stderr, "Error: failed to get minimal extensions for demo\n");
   	        fprintf(stderr, "This sample requires:\n");
   	        fprintf(stderr, "  OpenGL version 1.5\n");
   	        fprintf(stderr, "  GL_ARB_vertex_buffer_object\n");
   	        fprintf(stderr, "  GL_ARB_pixel_buffer_object\n");
   	        exit(EXIT_SUCCESS);
   	    }
	init();
   	glutMainLoop();
   	deInit();
}
