
__kernel void vector_dot_product(__global const float* source_a, __global const float* source_b, __global float* result, const int num)
{
	const int index = get_global_id(0);

	if (index < num)
		result[0] += (source_a[index] * source_b[index]);

}