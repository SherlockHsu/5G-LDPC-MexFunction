#include <mex.h>
#include <stdint.h>
#include <stdlib.h>

#include "ldpc.h"

/***************************************************************************
* Name:		mex_nr15_fec_ldpc_decoder_ms_punctured
*
* Function: ldpc min-sum decoder with nr15 puncturing
*
* Input:	prhs[0]:	received llr array
*			prhs[1]:	LDPC parameter array
*			prhs[2]:    H_col matrix generated by nr15_fec_ldpc_param_init.m
*			prhs[3]:    H_row matrix generated by nr15_fec_ldpc_param_init.m
*			prhs[4]:    max iteration
*
* Output:	plhs[0]:    decoded bits
***************************************************************************/
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    /* get prhs pointer */
    double *llr_D = mxGetPr(prhs[0]);
    double *ldpc_param_array_D = mxGetPr(prhs[1]);
    double *H_col_D = mxGetPr(prhs[2]);
    double *H_row_D = mxGetPr(prhs[3]);
    double I_max = mxGetScalar(prhs[4]);

    /* format input */
    int32_t *ldpc_param_array =
        (int32_t *)malloc(sizeof(int32_t) * SIZE_OF_LDPC_ARRAY);
    for (int i = 0; i < SIZE_OF_LDPC_ARRAY; i++)
        ldpc_param_array[i] = ldpc_param_array_D[i];
    nr15_ldpc_t *ldpc_param =
        (nr15_ldpc_t *)malloc(sizeof(nr15_ldpc_t));
    trans_array_to_nr15_ldpc_t(ldpc_param, ldpc_param_array);
    float *llr = (float *)malloc(sizeof(float) * ldpc_param->N);
    for (int i = 0; i < ldpc_param->N; i++)
        llr[i] = llr_D[i];
    int N = ldpc_param->N + 2 * ldpc_param->Z_c;
    ldpc_param->vn_list =
        (variable_node *)malloc(sizeof(variable_node) * N);
    for (int n = 0; n < N; n++)
    {
        ldpc_param->vn_list[n].degree = H_col_D[n];
        ldpc_param->vn_list[n].index =
            (int16_t *)malloc(sizeof(int16_t) * ldpc_param->vn_list[n].degree);
        for (int i = 0; i < ldpc_param->vn_list[n].degree; i++)
            ldpc_param->vn_list[n].index[i] = H_col_D[(i + 1) * N + n] - 1;
        ldpc_param->vn_list[n].message =
            (float *)malloc(sizeof(float) * ldpc_param->vn_list[n].degree);
    }
    int M = N - ldpc_param->K;
    ldpc_param->cn_list =
        (check_node *)malloc(sizeof(check_node) * M);
    for (int m = 0; m < M; m++)
    {
        ldpc_param->cn_list[m].degree = H_row_D[m];
        ldpc_param->cn_list[m].index =
            (int16_t *)malloc(sizeof(int16_t) * ldpc_param->cn_list[m].degree);
        for (int j = 0; j < ldpc_param->cn_list[m].degree; j++)
            ldpc_param->cn_list[m].index[j] = H_row_D[(j + 1) * M + m] - 1;
        ldpc_param->cn_list[m].message =
            (float *)malloc(sizeof(float) * ldpc_param->cn_list[m].degree);
    }

    /* decode */
    int8_t *decoded_bits =
        (int8_t *)malloc(sizeof(int8_t) * ldpc_param->K);
    nr15_fec_ldpc_decoder_ms_punctured(
        llr, ldpc_param, (int)I_max, decoded_bits);

    /* get plhs pointer */
    plhs[0] = mxCreateDoubleMatrix(1, ldpc_param->K, mxREAL);

    /* format output */
    double *output = mxGetPr(plhs[0]);
    for (int i = 0; i < ldpc_param->K; i++)
        output[i] = decoded_bits[i];

    free(ldpc_param_array);
    free_ldpc_param_with_node(ldpc_param);
    free(ldpc_param);
    free(llr);
    free(decoded_bits);
}