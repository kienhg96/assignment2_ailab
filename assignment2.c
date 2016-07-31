/*
WARNING:
	Compile this file with FLAG -lm to fix undefined reference to `pow'
	gcc -o linear main.c -lm 
*/

#include <stdio.h>
#include <math.h>
#define STEP 0.01
#define PRECISION 0.01

double square(double x){
    return x * x;
}

double f(double w[10], double xi, int M){ // f = w0 + w1*x + w2 * x2 ... 
    int i;
    double _xi = 1;
    double sum = 0;
    for (i = 0; i < M; i++){
        sum += _xi * w[i];
        _xi *= xi;
    }
    return sum;
}

double distance(double arr[10], int M){
    double s = 0;
    int i;
    for (i = 0; i < M; i++){
        s += arr[i] * arr[i];
    }
    return sqrt(s);
}

//double NablaE(double w[10], xi, )

int main(){
    FILE * file;
    double data[100][2];
    int i, j, l, M;
    double x_avg, y_avg, xy_avg, x2_avg;
    double w[10];
    double Erms = 0;
    double NablaE[10];
    int M_OPT;
    double Erms_OPT = 999999999;
    // Temp variable
    double _f, _x;
    
    file = fopen("learning_data2.txt", "r");
    for (i = 0; i < 100; i++){
        fscanf(file, "%lf%lf", &data[i][0], &data[i][1]);
    }
    fclose(file);
    // == Step 1 ==
    // Create Linear Regression
    printf("Step 1\n");
    printf("Create Linear Regression...\n");
    x_avg = y_avg = xy_avg = x2_avg = 0;
    for (i = 0; i < 100; i++){
        x_avg += data[i][0];
        y_avg += data[i][1];
        xy_avg += data[i][0] * data[i][1];
        x2_avg += data[i][0] * data[i][0];
    }
    x_avg /= 100; y_avg /= 100;
    xy_avg /= 100; x2_avg /= 100;
    w[1] = (xy_avg - x_avg * y_avg) / (x2_avg - x_avg * x_avg);
    w[0] = y_avg - w[1] * x_avg;
    printf("w0 = %.17f w1 = %.17f\n", w[0], w[1]);
    // Calculate RMS error by leave-one-out cross validation
    printf("Calculate RMS error by leave-one-out cross validation\n");
    Erms = 0;
    for (l = 0; l < 100; l++){
        for (i = 0; i < 100; i++){
            if (i != l){
                x_avg += data[i][0];
                y_avg += data[i][1];
                xy_avg += data[i][0] * data[i][1];
                x2_avg += data[i][0] * data[i][0];
            }
        }
        x_avg /= 99; y_avg /= 99;
        xy_avg /= 99; x2_avg /= 99;
        w[1] = (xy_avg - x_avg * y_avg) / (x2_avg - x_avg * x_avg);
        w[0] = y_avg - w[1] * x_avg;
        Erms += square(w[0] + w[1] * data[l][0] - data[l][1]);
    }
    Erms = sqrt(Erms / 100);
    printf("Erms = %.17f\n", Erms);
    
    // Step 2
    printf("\nStep 2\n");
    printf("Using Method of Steepest Decent\n");
    for (M = 2; M <= 10; M++){
        printf("M = %d\n", M);
        for (i = 0; i < M; i++){
            w[i] = 1;
        }
        do {
            for (i = 0; i < M; i++){
                NablaE[i] = 0;
            }
            for (i = 0; i < 100; i++){
                _f = f(w, data[i][0], M);
                _x = 1;
                for (j = 0; j < M; j++){
                    NablaE[j] += _x * (_f - data[i][1]);
                    _x *= data[i][0];
                }
            }
            for (i = 0; i < M; i++){
                w[i] -= STEP * NablaE[i];
            }
        } while (distance(NablaE, M) >= PRECISION);
        for (i = 0; i < M; i++){
            printf("w[%d] = %.17f\n",i,  w[i]);
        }
    }
    // Calc RMS error by Leave-one-out cross validate
    printf("\nCalc RMS error by Leave-one-out cross validate\n");
    Erms = 0;
    for (M = 2; M <= 10; M++){
        printf("M = %d ", M);
        for (l = 0; l < 100; l++){
            for (i = 0; i < M; i++){
                w[i] = 1;
            }
            do {
                for (i = 0; i < M; i++){
                    NablaE[i] = 0;
                }
                for (i = 0; i < 100; i++){
                    if (i != l){
                        _f = f(w, data[i][0], M);
                        _x = 1;
                        for (j = 0; j < M; j++){
                            NablaE[j] += _x * (_f - data[i][1]);
                            _x *= data[i][0];
                        }
                    }
                }
                for (i = 0; i < M; i++){
                    w[i] -= STEP * NablaE[i];
                }
            } while (distance(NablaE, M) >= PRECISION);
            Erms += square(f(w, data[l][0], M) - data[l][1]);
        }
        Erms = sqrt(Erms / 100);
        printf("Erms = %.17f\n", Erms);
        if (Erms < Erms_OPT){
            M_OPT = M;
            Erms_OPT = Erms;
        }
    }
    printf("\nOptimal M = %d\n", M_OPT);
    return 0;
}