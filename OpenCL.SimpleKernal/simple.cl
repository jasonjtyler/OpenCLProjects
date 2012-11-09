
__kernel void vector_dot_product(__global const float4* source_a, __global const float4* source_b, __global float* result, const int num)
{
	const int index = get_global_id(0);

	if (index < num) {
		result[index] = (source_a[index].x * source_b[index].x);
		result[index] += (source_a[index].y * source_b[index].y);
		result[index] += (source_a[index].z * source_b[index].z);
	}

}