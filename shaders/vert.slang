uniform matrix prjection_matrix;
uniform matrix view_matrix;
uniform matrix model_matrix;

[shader("vertex")]
float4 main(float3 position) {
    return mul(prjection_matrix * view_matrix * model_matrix, float4(position, 1.0f)); 
}