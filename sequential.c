// SEQUENTIAL CODE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

double calculate_mean(double data[], int size);
double calculate_variance(double data[], int size, double mean);
double calculate_median(double data[], int size);
void sort(double data[], int size);
double calculate_standard_deviation(double variance);
double calculate_range(double data[], int size);
double calculate_pearson(double x[], double y[], int size);

int main() {
    clock_t start, end;
    double cpu_time_used;

    start = clock();

    FILE *file = fopen("./country_wise_latest.csv", "r");
    if (file == NULL) {
        printf("Error: Could not open file\n");
        return 1;
    }

    double confirmed[200], deaths[200], recovered[200], active[200];
    double new_cases[200], new_deaths[200];
    double deaths_per_100_cases[200], recovered_per_100_cases[200], deaths_per_100_recovered[200];
    char buffer[1024];
    int row = 0, col = 0, data_count = 0;

    printf("\n%-10s %-10s %-10s %-15s %-10s %-10s %-10s\n", 
           "Row", "Mean", "Median", "Variance", "Stddev", "Range", "Pearson");

    while (fgets(buffer, 1024, file)) {
        col = 0;
        row++;

        if (row == 1) continue;

        char *field = strtok(buffer, ",");
        double row_data[11];

        while (field) {
            if (col >= 1 && col <= 11) {  
                row_data[col-1] = atof(field);
            }
            field = strtok(NULL, ",");
            col++;
        }

        double mean = calculate_mean(row_data, 11);
        double median = calculate_median(row_data, 11);
        double variance = calculate_variance(row_data, 11, mean);
        double stddev = calculate_standard_deviation(variance);
        double range = calculate_range(row_data, 11);
        double pearson = calculate_pearson(row_data, &row_data[1], 10);  

        printf("%-10d %-10.2f %-10.2f %-15.2f %-10.2f %-10.2f %-10.2f\n", 
               row - 1, mean, median, variance, stddev, range, pearson);

        data_count++;
    }

    fclose(file);

    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Program Execution Time [sequential]: %f seconds\n", cpu_time_used);

    return 0;
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

double calculate_standard_deviation(double variance) {
    return sqrt(variance);
}

double calculate_median(double data[], int size) {
    sort(data, size);
    if (size % 2 == 0) {
        return (data[size / 2 - 1] + data[size / 2]) / 2.0;
    } else {
        return data[size / 2];
    }
}

void sort(double data[], int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = i + 1; j < size; j++) {
            if (data[i] > data[j]) {
                double temp = data[i];
                data[i] = data[j];
                data[j] = temp;
            }
        }
    }
}

double calculate_range(double data[], int size) {
    double min = data[0], max = data[0];
    for (int i = 1; i < size; i++) {
        if (data[i] < min) min = data[i];
        if (data[i] > max) max = data[i];
    }
    return max - min;
}

double calculate_pearson(double x[], double y[], int size) {
    double sum_x = 0.0, sum_y = 0.0, sum_x_sq = 0.0, sum_y_sq = 0.0, sum_xy = 0.0;

    for (int i = 0; i < size; i++) {
        sum_x += x[i];
        sum_y += y[i];
        sum_x_sq += x[i] * x[i];
        sum_y_sq += y[i] * y[i];
        sum_xy += x[i] * y[i];
    }

    double numerator = size * sum_xy - sum_x * sum_y;
    double denominator = sqrt((size * sum_x_sq - sum_x * sum_x) * (size * sum_y_sq - sum_y * sum_y));

    return (denominator == 0) ? 0 : numerator / denominator;
}
