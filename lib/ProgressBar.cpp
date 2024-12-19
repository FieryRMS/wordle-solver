#include "ProgressBar.h"
#include <iostream>
using namespace std;

void ProgressBar::update(ull progress)
{
    int pos = (width * progress) / total;
    int percent = (100 * progress) / total;
    if (pos == lastPos && percent == lastPercent) return;
    lastPos = pos, lastPercent = percent;

    cout << "[";
    for (int i = 0; i < width; ++i)
    {
        if (i < pos) cout << "=";
        else if (i == pos) cout << ">";
        else cout << " ";
    }
    cout << "] " << percent << " %\r";
    cout.flush();
}

void ProgressBar::finish()
{
    update(total);
    cout << endl;
}