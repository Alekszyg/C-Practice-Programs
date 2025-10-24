#include <stdio.h>
#include <math.h>

float chance_at_least_once(float probability, int tries);
double chance_at_least_n(double probability, int tries, int n);
double choose(int n, int k);

int main()
{
    // probability of an event in percent
    double probability_percent = 1;
    int tries = 100;

    double probability = probability_percent / 100.0;

    printf("\n%.10f%%", chance_at_least_n(probability, tries, 2));
    return 0;
}

// returns chance that event happened at least n times in n number of tries
double chance_at_least_n(double probability, int tries, int n)
{
    double p = probability;
    double q = 1.0 - p;

    double cumulative = 0.0;

    for (int i = 0; i < n; i++)
    {
        cumulative += choose(tries, i) * pow(p, i) * pow(q, tries - i);
    }

    double final_probability = 1.0 - cumulative;

    return 100.0 * final_probability;  // Return as percent

}


double choose(int n, int k) {
    if (k > n || k < 0) return 0;
    if (k == 0) return 1;
    return tgamma(n + 1) / (tgamma(k + 1) * tgamma(n - k + 1));
}
