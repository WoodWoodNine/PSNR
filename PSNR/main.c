//
//  main.c
//  PSNR
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define uint8_t unsigned char
float psnr(uint8_t* p1, uint8_t* p2, int size);
void mReturn(void);
uint8_t * buf1, *buf2;
FILE* in_file1, *in_file2, *out_file;

int main(int argc, char* argv[])
{
    int frame_no;
    int size, luma_size, chroma_size;
    int height, width, frame_num;
    float y = 0.0, u = 0.0, v = 0.0;
    buf1=NULL;
    buf2=NULL;
    if (argc!=7)
    {
        printf("invalid input parameter!\n\n");
        printf("help: Format example\nPSNR.exe ref.yuv input.yuv output.txt width height frame_num\n");
        return 0;
    }
    
    width = atoi(argv[4]);
    height = atoi(argv[5]);
    frame_num = atoi(argv[6]);
    luma_size = height * width;
    size = luma_size + (luma_size >> 1);
    chroma_size = luma_size / 4;
    
    buf1 = (uint8_t *)malloc(size);
    buf2 = (uint8_t *)malloc(size);
    
    in_file1 = fopen(argv[1], "rb");
    if (!in_file1)
    {
        printf("cannot open input file %s\n", argv[1]);
        mReturn();
    }
    in_file2 = fopen(argv[2], "rb");
    if (!in_file2)
    {
        printf("cannot open input file  %s\n", argv[2]);
        mReturn();
    }
    out_file = fopen(argv[3], "at");
    if (!out_file)
    {
        printf("cannot open output file  %s\n", argv[3]);
        mReturn();
    }
    
    printf(" == PSNR.exe =====================================\n");
    printf(" reference file : %s\n", argv[1]);
    printf(" input file     : %s\n", argv[2]);
    printf(" output file    : %s\n", argv[3]);
    printf(" width          : %d\n", width);
    printf(" height         : %d\n", height);
    printf(" frame number   : %d\n", frame_num);
    printf(" =================================================\n");
    printf(" == Calculate PSNR ===============================\n");
    for(frame_no = 0; frame_no < frame_num; frame_no++ )
    {
        printf("%d ", frame_no);
        if (fread(buf1, size, 1, in_file1) <= 0)
        {
            printf("cannot read from input file.");
            break;
        }
        if (fread(buf2, size, 1, in_file2) <= 0)
        {
            printf("cannot read from input file.");
            break;
        }
        
        y += psnr(buf1, buf2, luma_size);
        u += psnr(buf1 + luma_size, buf2 + luma_size, chroma_size);
        v += psnr(buf1 + luma_size + chroma_size, buf2 + luma_size + chroma_size, chroma_size);
    }
    
    printf("\n");
    printf(" =================================================\n");
    printf(" PSNR-Y : %.4f \n", y/frame_num);
    printf(" PSNR-U : %.4f \n", u/frame_num);
    printf(" PSNR-V : %.4f \n", v/frame_num);
    fprintf(out_file, "%s\t%s\t", argv[1], argv[2]);
    fprintf(out_file, "%.4f\t%.4f\t%.4f\n", y/frame_num, u/frame_num, v/frame_num);
    printf(" Results are written to %s\n", argv[3]);
    printf(" =================================================\n");
    
    mReturn();
    return 0;
}

void mReturn(void)
{
    if(!in_file1) fclose(in_file1);
    if(!in_file2) fclose(in_file2);
    if(!out_file) fclose(out_file);
    if(!buf1) free(buf1);
    if(!buf2) free(buf2);
}

float psnr(uint8_t* p1, uint8_t* p2, int size)
{
    unsigned long sad = 0;
    int i;
    
    for (i = 0 ; i < size ; i ++)
    {   
        int tmp;   
        tmp = (p1[i] - p2[i]);   
        sad += tmp * tmp;   
    }   
    
    return (float)(10 * log10(65025.0f * size/sad));   
}  
