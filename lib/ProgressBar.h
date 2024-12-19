#pragma once

typedef unsigned long long ull;
class ProgressBar {
   public:
    ProgressBar(ull total, int width = 70) : total(total), width(width) {}

    void update(unsigned long long progress);
    void setTotal(ull t) { this->total = t; }
    void finish();

   private:
    ull total;
    int width;
    int lastPos = -1;
    int lastPercent = -1;
};