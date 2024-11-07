#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

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
    for (int i = 0; i < size; i++) {
        sum += data[i];
    }
    return sum / size;
}

double calculate_variance(double data[], int size, double mean) {
    double sum = 0.0;
    for (int i = 0; i < size; i++) {
        sum += (data[i] - mean) * (data[i] - mean);
    }
    return sum / size;
}

double calculate_median(double data[], int size) {
    double* temp = malloc(size * sizeof(double));
    memcpy(temp, data, size * sizeof(double));
    sort(temp, size);
    double median;
    if (size % 2 == 0) {
        median = (temp[size / 2 - 1] + temp[size / 2]) / 2.0;
    } else {
        median = temp[size / 2];
    }
    free(temp);
    return median;
}

double calculate_standard_deviation(double variance) {
    return sqrt(variance);
}

double calculate_range(double data[], int size) {
    double min = data[0];
    double max = data[0];
    for (int i = 1; i < size; i++) {
        if (data[i] < min) min = data[i];
        if (data[i] > max) max = data[i];
    }
    return max - min;
}

int main() {
    clock_t start, end;
    double cpu_time_used;
    
    start = clock(); // Start timing

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

    for (int i = 0; i < rows; i++) {
        double mean = calculate_mean(data[i], cols);
        double variance = calculate_variance(data[i], cols, mean);
        double median = calculate_median(data[i], cols);
        double std_dev = calculate_standard_deviation(variance);
        double range = calculate_range(data[i], cols);

        printf("%-10d %-15.6f %-15.6f %-15.6f %-15.6f %-15.6f\n",
               i, mean, variance, median, std_dev, range);
    }

    // Free allocated memory
    for (int i = 0; i < rows; i++) {
        free(data[i]);
    }
    free(data);

    end = clock(); // End timing
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("\nExecution time: %f seconds\n", cpu_time_used);

    return 0;
}