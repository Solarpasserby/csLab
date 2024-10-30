#include <stdio.h>
#include <stdlib.h>

#define KEY 0xAB

void encrypt_file(const char *input_file, const char *output_file)
{
    FILE *in = fopen(input_file, "rb");
    FILE *out = fopen(output_file, "wb");
    if (!in || !out)
    {
        perror("fail to open file");
        exit(1);
    }

    unsigned char buffer[1024];
    int n;

    while ((n = fread(buffer, sizeof(unsigned char), 1024, in)) > 0)
    {
        for (int i = 0; i < n; i++)
            buffer[i] ^= KEY; // enpcript
        fwrite(buffer, 1, n, out);
    }

    fclose(in);
    fclose(out);
}

void decrypt_file(const char *input_file, const char *output_file)
{
    encrypt_file(input_file, output_file);
}

int compare_files(const char *file1, const char *file2)
{
    FILE *f1 = fopen(file1, "rb");
    FILE *f2 = fopen(file2, "rb");
    if (!f1 || !f2)
    {
        perror("fail to open file");
        exit(1);
    }

    int result = 1;
    int c1, c2;
    while ((c1 = fgetc(f1)) != EOF && (c2 = fgetc(f2)) != EOF)
    {
        if (c1 != c2)
        {
            result = 0;
            break;
        }
    }
    if (fgetc(f1) != EOF || fgetc(f2) != EOF)
    {
        result = 0;
    }

    fclose(f1);
    fclose(f2);
    return result;
}

int main()
{
    const char *source_file = "source_file.txt";
    const char *encryption_file = "encryption_file.txt";
    const char *decryption_file = "decryption_file.txt";

    printf("0. Exit\n");
    printf("1. Encrypt the file\n");
    printf("2. Decrypt the file\n");
    printf("3. Compare the files\n");
    fflush(stdout);

    char ch;
    ch = getchar();

    while (ch != '0')
    {
        switch (ch)
        {
        case '1':
            encrypt_file(source_file, encryption_file);
            printf("The sourcefile has been encrypted and saved at %s\n", encryption_file);
            break;
        case '2':
            decrypt_file(encryption_file, decryption_file);
            printf("The sourcefile has been decrypted and saved at %s\n", decryption_file);
            break;
        case '3':
            if (compare_files(source_file, decryption_file))
            {
                printf("Verification Success!\n");
            }
            else
            {
                printf("Verification Failure!\n");
            }
            break;
        default:
            printf("Invalid input\n");
            break;
        }

        fflush(stdout);
        getchar();
        ch = getchar();
    }

    return 0;
}
