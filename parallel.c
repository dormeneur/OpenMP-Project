#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <omp.h>

#define MAX_LINE_LENGTH 1024

void sort(double data[], int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (data[j] > data[j + 1]) {
                double temp = data[j];
                data[j] = data[j + 1];
                data[j + 1] = temp;
            }
        }
    }
}

double calculate_mean(double data[], int size) {
    double sum = 0.0;
    #pragma omp simd reduction(+:sum)
    for (int i = 0; i < size; i++) {
        sum += data[i];
    }
    return sum / size;
}

double calculate_variance(double data[], int size, double mean) {
    double sum = 0.0;
    #pragma omp simd reduction(+:sum)
    for (int i = 0; i < size; i++) {
        double diff = data[i] - mean;
        sum += diff * diff;
    }
    return sum / size;
}

double calculate_median(double data[], int size) {
    double* temp = malloc(size * sizeof(double));
    memcpy(temp, data, size * sizeof(double));
    sort(temp, size);
    double median = (size % 2 == 0) ? (temp[size/2 - 1] + temp[size/2]) / 2.0 : temp[size/2];
    free(temp);
    return median;
}

double calculate_range(double data[], int size) {
    double min = data[0], max = data[0];
    #pragma omp simd reduction(min:min) reduction(max:max)
    for (int i = 1; i < size; i++) {
        if (data[i] < min) min = data[i];
        if (data[i] > max) max = data[i];
    }
    return max - min;
}

int main() {
    double start_time, end_time;
    
    start_time = omp_get_wtime(); // Start timing

    FILE *file = fopen("./AEP_hourly.csv", "r");
    if (!file) {
        perror("Unable to open file");
        return 1;
    }

    int rows = 121274; // Set the number of rows
    int cols = 2; // Set the number of columns
    double **data = malloc(rows * sizeof(double*));
    for (int i = 0; i < rows; i++) {
        data[i] = malloc(cols * sizeof(double));
    }

    char line[MAX_LINE_LENGTH];
    int row = 0;
    while (fgets(line, MAX_LINE_LENGTH, file) && row < rows) {
        char *token = strtok(line, ",");
        for (int col = 0; col < cols && token != NULL; col++) {
            data[row][col] = atof(token);
            token = strtok(NULL, ",");
        }
        row++;
    }

    fclose(file);

    // Print table header
    printf("%-10s %-15s %-15s %-15s %-15s %-15s\n", 
           "Row", "Mean", "Variance", "Median", "Std Dev", "Range");
    printf("-------------------------------------------------------------------------------------\n");

    #pragma omp parallel
    {
        #pragma omp for schedule(dynamic)
        for (int i = 0; i < rows; i++) {
            double mean = calculate_mean(data[i], cols);
            double variance = calculate_variance(data[i], cols, mean);
            double median = calculate_median(data[i], cols);
            double std_dev = sqrt(variance);
            double range = calculate_range(data[i], cols);

            #pragma omp critical
            {
                printf("%-10d %-15.6f %-15.6f %-15.6f %-15.6f %-15.6f\n",
                       i, mean, variance, median, std_dev, range);
            }
        }
    }

    // Free allocated memory
    for (int i = 0; i < rows; i++) {
        free(data[i]);
    }
    free(data);

    end_time = omp_get_wtime(); // End timing
    
    printf("\nExecution time: %f seconds\n", end_time - start_time);

    return 0;
}