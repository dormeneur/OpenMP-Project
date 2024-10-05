<<<<<<< HEAD
// PARALLEL CODE (OpenMP)
=======
// PARALLEL CODE
>>>>>>> ceb2c9449702a767ffb592c5e5306adb699ec2aa
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <omp.h>

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

<<<<<<< HEAD
    printf("\n%-10s %-10s %-10s %-15s %-10s %-10s %-10s\n", 
           "Row", "Mean", "Median", "Variance", "Stddev", "Range", "Pearson");
=======
    // Print the table header
    printf("\n%-10s %-10s %-10s %-15s %-10s %-10s %-10s\n", 
           "Row", "Mean", "Median", "Variance", "Stddev", "Range", "Pearson");

    // Reading the CSV file line by line
    while (fgets(buffer, 1024, file)) {
        col = 0;
        row++;
>>>>>>> ceb2c9449702a767ffb592c5e5306adb699ec2aa

    #pragma omp parallel private(buffer, col)  // Parallelize loop over rows
    {
        while (fgets(buffer, 1024, file)) {
            col = 0;
            row++;

<<<<<<< HEAD
            if (row == 1) continue;

            char *field = strtok(buffer, ",");
            double row_data[11];

            while (field) {
                if (col >= 1 && col <= 11) {  
                    row_data[col-1] = atof(field);
                }
                field = strtok(NULL, ",");
                col++;
=======
        char *field = strtok(buffer, ",");
        double row_data[11];  // To store all numeric fields for this row

        // Parse the required fields
        while (field) {
            if (col >= 1 && col <= 11) {  // We're interested in columns 1-11
                row_data[col-1] = atof(field);
>>>>>>> ceb2c9449702a767ffb592c5e5306adb699ec2aa
            }

            double mean, median, variance, stddev, range, pearson;

            #pragma omp sections
            {
                #pragma omp section
                mean = calculate_mean(row_data, 11);

                #pragma omp section
                median = calculate_median(row_data, 11);

                #pragma omp section
                variance = calculate_variance(row_data, 11, mean);

                #pragma omp section
                stddev = calculate_standard_deviation(variance);

                #pragma omp section
                range = calculate_range(row_data, 11);

                #pragma omp section
                pearson = calculate_pearson(row_data, &row_data[1], 10);
            }

            #pragma omp critical
            printf("%-10d %-10.2f %-10.2f %-15.2f %-10.2f %-10.2f %-10.2f\n", 
                   row - 1, mean, median, variance, stddev, range, pearson);

            data_count++;
        }
<<<<<<< HEAD
=======

        // Calculate statistics for this row
        double mean = calculate_mean(row_data, 11);
        double median = calculate_median(row_data, 11);
        double variance = calculate_variance(row_data, 11, mean);
        double stddev = calculate_standard_deviation(variance);
        double range = calculate_range(row_data, 11);
        double pearson = calculate_pearson(row_data, &row_data[1], 10);  // Correlation between confirmed and deaths

        // Print the results for this row
        printf("%-10d %-10.2f %-10.2f %-15.2f %-10.2f %-10.2f %-10.2f\n", 
               row - 1, mean, median, variance, stddev, range, pearson);

        data_count++;
>>>>>>> ceb2c9449702a767ffb592c5e5306adb699ec2aa
    }

    fclose(file);

    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Program Execution Time [parallel]: %f seconds\n", cpu_time_used);

    return 0;
}

<<<<<<< HEAD
=======
// Function implementations remain the same as in the original code
// ...

// Function to calculate the mean
>>>>>>> ceb2c9449702a767ffb592c5e5306adb699ec2aa
double calculate_mean(double data[], int size) {
    double sum = 0.0;

    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < size; i++) {
        sum += data[i];
    }

    return sum / size;
}

double calculate_variance(double data[], int size, double mean) {
    double sum = 0.0;

    #pragma omp parallel for reduction(+:sum)  
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

    #pragma omp parallel for reduction(min:min) reduction(max:max)
    for (int i = 1; i < size; i++) {
        if (data[i] < min) min = data[i];
        if (data[i] > max) max = data[i];
    }

    return max - min;
}

double calculate_pearson(double x[], double y[], int size) {
    double sum_x = 0.0, sum_y = 0.0, sum_x_sq = 0.0, sum_y_sq = 0.0, sum_xy = 0.0;

    #pragma omp parallel for reduction(+:sum_x, sum_y, sum_x_sq, sum_y_sq, sum_xy)
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
