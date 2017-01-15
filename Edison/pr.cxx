// -std=c++11


#include <unistd.h>
//! [Interesting]
#include "mraa/gpio.h"
#include "mraa/aio.h"
#include "jhd1313m1.hpp"
#include <math.h>
#include <chrono>
#include <cstdlib>
#include <queue>
#include <vector>
#include <iostream>
#include <python2.7/Python.h>
#include <fstream>

/* first, create our counting variable */
volatile size_t cur_roter = 0;
volatile int cur_cur_but = 0;
volatile int cur_blink = 0;
volatile int cur_sens = 0;
volatile size_t cur_max = ULONG_MAX;
std::chrono::steady_clock::time_point timer = std::chrono::steady_clock::now();
std::queue<size_t> que;

/* Now our simple counting function. */
/* This will be our interrupt handler. */


void pythonScript(const std::vector<double> &raw_value) {
    std::ifstream fcheck("temp.txt");
    if (fcheck.is_open()) {
        fcheck.close();
        return;
    }
    
    std::ofstream fout("temp.txt");    
    std::string buf = "";
    for (int i = 0; i < raw_value.size()-1; ++i) {
        buf += std::to_string((size_t)raw_value[i]) + "-";
    }
    buf += std::to_string((size_t)raw_value[raw_value.size()-1]);
    std::cout<<buf<<std::endl;
    fout << buf << std::endl;
    fout.close();
}

void intrHandlerBut(void *arg)
{
    cur_cur_but = !cur_cur_but;
    cur_blink = !cur_blink;
    if (cur_blink) {
        cur_max = ULONG_MAX;
        printf("On\n");
    } else {
        printf("Off\n");
        size_t r = cur_roter;
        if (que.size() < 2)
            return;
        que.pop();
        
        size_t size = que.size() / r;
        std::vector<double> raw_value(size, 0);
        
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < r; ++j) {
                raw_value[i] += que.front() ;// / 1000.0;
                que.pop();
            }
            raw_value[i] /= (double)r;
        }
        // send raw_value to python
        pythonScript(raw_value);
        while (!que.empty()) 
            que.pop();
    }
    printf("Roter: %d\n", cur_roter);
}
void intrHandlerSens(void *arg)
{
    std::chrono::steady_clock::time_point n_timer = std::chrono::steady_clock::now();
    size_t delta = n_timer.time_since_epoch().count() - timer.time_since_epoch().count();
    const size_t filter = 10000000;
    if (cur_cur_but && delta > filter) {
        que.push(delta);
        if (delta <  cur_max) {
            cur_max = delta;
        }
        std::cout<<"Blinks: "<<que.size()<<"; "<<delta<<std::endl;
    }
    timer = std::chrono::steady_clock::now();
}
void p_lsd(upm::Jhd1313m1 &lcd) {
    
}
int main() {

    /* initialize MRAA */
	mraa_init();

	/* create an MRAA digital context */
    mraa_gpio_context m_gpio_but;
    mraa_gpio_context m_gpio_sens;
    mraa_gpio_context m_gpio_blink;

    /* create an MRAA analog context */
    mraa_aio_context m_aio_roter;

	/* initialize pins */
	m_gpio_but = mraa_gpio_init(2);
    m_gpio_blink = mraa_gpio_init(4);
    m_gpio_sens = mraa_gpio_init(6);
    m_aio_roter = mraa_aio_init(1);


    /* configure the digital pin as an input */
	mraa_gpio_dir(m_gpio_but, MRAA_GPIO_IN);
    mraa_gpio_dir(m_gpio_sens, MRAA_GPIO_IN);
    mraa_gpio_dir(m_gpio_blink, MRAA_GPIO_OUT);

    /* now, setup an interrupt handler. */
	/* Our function (intrHandler()) above will */
	/* be called whenever the pin goes from */
	/* HIGH to LOW */

    mraa_gpio_isr(m_gpio_but, MRAA_GPIO_EDGE_RISING, &intrHandlerBut, NULL);
    mraa_gpio_isr(m_gpio_sens, MRAA_GPIO_EDGE_RISING, &intrHandlerSens, NULL);

    upm::Jhd1313m1 *lcd = new upm::Jhd1313m1(0, 0x3E, 0x62);
    char string[100] = {};
	/* sleep and then print out the current */
    int rt = -2;
    size_t st = 1;
    for (;;) {
		usleep(100000);
        if (cur_blink) {
            mraa_gpio_write(m_gpio_blink, 1);
        } else {
            mraa_gpio_write(m_gpio_blink, 0);
        }
        //cur_color = mraa_gpio_read(m_gpio_color);
        cur_roter = mraa_aio_read(m_aio_roter) / 100 + 1;
        
        if (st != cur_max) {
            st = cur_max;
            sprintf(string, "Speed: %.2f m/s", (25120000.0 / (double)cur_max));
            lcd->setCursor(0,0);
            lcd->write("              ");
            lcd->setCursor(0,0);
            lcd->write(string);
        }

        if (rt != cur_roter) {
            rt = cur_roter;
            sprintf(string, "Roter: %d", cur_roter);
            lcd->setCursor(1,0);
            lcd->write("              ");
            lcd->setCursor(1,0);
            lcd->write(string);
        }
	}

    return MRAA_SUCCESS;
}
//! [Interesting]
