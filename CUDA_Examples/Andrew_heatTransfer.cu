/* *
 * Copyright 1993-2012 NVIDIA Corporation.  All rights reserved.
 *
 * Please refer to the NVIDIA end user license agreement (EULA) associated
 * with this source code for terms and conditions that govern your use of
 * this software. Any use, reproduction, disclosure, or distribution of
 * this software and related documentation outside the terms of the EULA
 * is strictly prohibited.
 */
#define GL_GLEXT_PROTOTYPES
#include <cuda.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

#define rnd(x) (x*rand() / RAND_MAX)
#define SPEED 1.0f
#define MAX_TEMP 1.0f
#define MIN_TEMP 0.0001f
#define INF 2e10f

const int window_width = 1024;
const int window_height = 1024;

//The pixel buffer object id that is used to address the pixel buffer that
//is created on the GPU
GLuint gl_PBO;
//The cuda graphics resource that is used to link the OpenGL pixel buffer
//to CUDA
cudaGraphicsResource *cuda_pbo_resource;
// the pointer used to store the GPU address to the pixel buffer object
// to give to the kernel for computation
texture<float,2> texConstSrc;
texture<float,2> texIn;
texture<float,2> texOut;

float *dev_inSrc;
float *dev_outSrc;
float *dev_constSrc;
uchar4 *d_dst = NULL;

dim3 blockSize;
dim3 gridSize;

__global__ void copy_const_kernel( float *iptr){
	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;
	int offset = x + y * blockDim.x * gridDim.x;

	float c = tex2D(texConstSrc,x,y);
	if(c != 0){
		iptr[offset] = c;
	}
}

__global__ void transferHeat(float * out, bool dstOut){
	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;
	int offset = x + y * blockDim.x * gridDim.x;

	float t, l, c, r, b, tl, tr, bl, br;
	if(dstOut){
		t = tex2D(texIn,x,y-1);
		l = tex2D(texIn,x-1, y);
		c = tex2D(texIn,x,y);
		r = tex2D(texIn,x+1, y);
		b = tex2D(texIn,x,y+1);
		tl = tex2D(texIn, x-1, y-1);
		tr = tex2D(texIn, x+1, y-1);
		bl = tex2D(texIn, x-1, y+1);
		br = tex2D(texIn, x+1, y+1);
	}else{
		t = tex2D(texOut,x,y-1);
		l = tex2D(texOut,x-1, y);
		c = tex2D(texOut,x,y);
		r = tex2D(texOut,x+1, y);
		b = tex2D(texOut,x,y+1);
		tl = tex2D(texOut, x-1, y-1);
		tr = tex2D(texOut, x+1, y-1);
		bl = tex2D(texOut, x-1, y+1);
		br = tex2D(texOut, x+1, y+1);
	}

	out[offset] = c + ((t + b + l + r + tl + tr + bl + br)/8 - c);


}

__global__ void float_to_color(uchar4 * pixels, float* in){
	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;
	int offset = x + y * blockDim.x * gridDim.x;

	float num = in[offset];

	pixels[offset].x = (int)(num*255);
	pixels[offset].y = (int)(0);
	pixels[offset].z = (int)((MAX_TEMP-num) * 255);
	pixels[offset].w = 255;
}

void display(void){
	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	glFlush();

	cudaGraphicsMapResources(1, &cuda_pbo_resource, NULL);

	cudaEventRecord(start, 0);
	volatile bool dstOut = false;
	for(int i = 0; i < 200; i++){
		float *in, *out;
		if(dstOut){
			in = dev_inSrc;
			out = dev_outSrc;
		} else {
			out = dev_inSrc;
			in = dev_outSrc;
		}
		copy_const_kernel<<<gridSize, blockSize>>>(in);
		transferHeat<<<gridSize, blockSize>>>(out, dstOut);
		dstOut = !dstOut;
	}
	float_to_color<<<gridSize, blockSize>>>(d_dst, dev_inSrc);

	cudaEventRecord(stop, 0);
	cudaEventSynchronize(stop);

	cudaGraphicsUnmapResources(1, &cuda_pbo_resource, 0);

	glDrawPixels(window_width, window_height, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glutSwapBuffers();

	float elapsedTime;
	cudaEventElapsedTime(&elapsedTime, start, stop);

	char title[20];
	sprintf(title,"time: %3.2f", elapsedTime/200);

	glutSetWindowTitle(title);

	cudaEventDestroy(start);
	cudaEventDestroy(stop);

}

void init(){
	blockSize.x = 16;
	blockSize.y = 16;
	blockSize.z = 1;

	gridSize.x = window_width/16;
	gridSize.y = window_height/16;
	gridSize.z = 1;

	glGenBuffers(1, &gl_PBO);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, gl_PBO);
	glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, window_width*window_height*sizeof(uchar4), NULL, GL_DYNAMIC_DRAW_ARB);

	cudaGraphicsGLRegisterBuffer(&cuda_pbo_resource, gl_PBO, cudaGraphicsMapFlagsNone);
	cudaGraphicsMapResources(1, &cuda_pbo_resource, NULL);
	size_t num_bytes;
	cudaGraphicsResourceGetMappedPointer((void**)&d_dst, &num_bytes, cuda_pbo_resource);

	cudaGraphicsUnmapResources(1, &cuda_pbo_resource, 0);

	cudaMalloc((void**)&dev_inSrc, window_width*window_height*sizeof(float));
	cudaMalloc((void**)&dev_outSrc, window_width*window_height*sizeof(float));
	cudaMalloc((void**)&dev_constSrc, window_width*window_height*sizeof(float));

	cudaChannelFormatDesc desc = cudaCreateChannelDesc<float>();

	cudaBindTexture2D(NULL, texConstSrc, dev_constSrc, desc, window_width, window_height, sizeof(float)*window_width);
	cudaBindTexture2D(NULL, texIn, dev_inSrc, desc, window_width, window_height, sizeof(float)*window_width);
	cudaBindTexture2D(NULL, texOut, dev_outSrc, desc, window_width, window_height, sizeof(float)*window_width);

	float *temp = (float*)malloc(window_width*window_height*sizeof(float));
	for(int i = 0; i < window_width*window_height; i++){
		temp[i] = 0;
		int x = i % window_width;
		int y = i / window_height;
		if((x > 300) && (x < 600) && (y > 310) && (y < 601)){
			temp[i] = MAX_TEMP;
		}
	}
	temp[window_width*100+100] = (MAX_TEMP + MIN_TEMP) / 2;
	temp[window_width*700+100] = MIN_TEMP;
	temp[window_width*300+300] = MIN_TEMP;
	temp[window_width*200+700] = MIN_TEMP;
	for(int y=800; y < 900; y++){
		for(int x=400; x<500; x++){
			temp[x+y*window_width] = MIN_TEMP;
		}
	}
	cudaMemcpy(dev_constSrc, temp, window_width*window_height*sizeof(float), cudaMemcpyHostToDevice);
	for(int i = 0; i < window_width*window_height; i++){
		temp[i] = MIN_TEMP;
	}
	cudaMemcpy(dev_inSrc, temp, window_width*window_height*sizeof(float), cudaMemcpyHostToDevice);
	cudaMemcpy(dev_outSrc, dev_inSrc, window_width*window_height*sizeof(float), cudaMemcpyDeviceToDevice);
	free(temp);

}

void deInit(){
	cudaGraphicsUnmapResources(1, &cuda_pbo_resource, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
	glDeleteBuffers(1, &gl_PBO);

	cudaUnbindTexture(texIn);
	cudaUnbindTexture(texOut);
	cudaUnbindTexture(texConstSrc);

	cudaFree(dev_inSrc);
	cudaFree(dev_outSrc);
	cudaFree(dev_constSrc);
}

void keyboard(unsigned char key, int x, int y){
	switch(key){
	case ' ':;
		glutPostRedisplay();
		break;
	}
}

void idle(void){
	glutPostRedisplay();
}

int main(int argc, char** argv) {
	cudaDeviceProp prop;
	int dev;

	memset(&prop, 0, sizeof(cudaDeviceProp));
	prop.major = 1;
	prop.minor = 0;
	cudaChooseDevice(&dev, &prop);
	cudaGLSetGLDevice(dev);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("Ray Tracing Renderer");
	glutIdleFunc(idle);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);

	init();
	glutMainLoop();
	deInit();

	return 0;
}
