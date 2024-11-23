#pragma once

class ProgressBar {
   public:
    ProgressBar(int total, int width = 70) : total(total), width(width) {}

    void update(int progress);
    void setTotal(int t) { this->total = t; }
    void finish();

   private:
    int total;
    int width;
    int lastPos = -1;
    int lastPercent = -1;
};