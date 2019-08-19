#ifdef COMPUTE

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) buffer TestBuffer
{
	int data_SSBO[];
};

void main()
{
	data_SSBO[gl_GlobalInvocationID.x] = -data_SSBO[gl_GlobalInvocationID.x];
}

#endif