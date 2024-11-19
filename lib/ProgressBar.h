#pragma once

class ProgressBar {
   public:
    ProgressBar(int total, int width = 50) : total(total), width(width) {}

    void update(int progress);
    void finish();

   private:
    int total;
    int width;
    int lastPos = -1;
    int lastPercent = -1;
};