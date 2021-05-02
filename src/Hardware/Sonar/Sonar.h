#ifndef __SONAR_H__
#define __SONAR_H__

#include "mbed.h"
#include <chrono>
#include <ratio>

class Sonar
{
private:
  static constexpr chrono::microseconds MAX_ECHO_US = 23200us;
  enum class Stauts {FREE,MEASURING,SENDING};
  Stauts sonar_status;
  InterruptIn echo;
  DigitalOut trigger;
  Timer sonar;
  Timeout time_out;
  int correction;
  Callback<void(int)> deal_result;
  
  static int inline_read_us(Timer& target) //内联读取计时器时间（消除warning）
  {
    return std::chrono::duration<int, micro>(target.elapsed_time()).count();
  }

  void DealTimeOut()
  {
    switch(sonar_status)
    {
    case Stauts::MEASURING:  //等待返回内容时超时，即模块未发出 ECHO 信号
      deal_result(-2);
      break;
    case Stauts::SENDING: //测量输出引脚高电平时长时超时，即距离过远
      deal_result(-1);
      break;
    default:  //未知错误
      deal_result(-3);
      break;
    }
    sonar_status=Stauts::FREE;
  }

  void BeginCalculate()
  {
    sonar.start();
    sonar_status=Stauts::SENDING;
  }

  void FinishCalculate()
  {
    sonar.stop();
    time_out.detach();
    deal_result((inline_read_us(sonar) - correction) / 5.8);
    echo.rise(nullptr);
    echo.fall(nullptr);
    sonar_status=Stauts::FREE;
  }

public:
  Sonar(PinName p_trigger, PinName p_echo, Callback<void(int)> deal_function)
      : trigger(p_trigger,PullDown), echo(p_echo, PullNone), sonar_status(Stauts::FREE), deal_result(deal_function)
  {
    sonar.reset();
    sonar.start();
    sonar.stop();
    correction = inline_read_us(sonar);
  }
  // 回调返回测量结果(mm)
  // 特别的：
  // -2 模块未发出 ECHO 信号（可能未连接）
  // -1 距离过远
  void detect_distance()
  {
    sonar.reset();
    sonar_status=Stauts::MEASURING;
    time_out.attach([this]{this->DealTimeOut();},MAX_ECHO_US);
    echo.rise([this]{this->BeginCalculate();});
    echo.fall([this]{this->FinishCalculate();});
    trigger = 1;
    wait_us(10.0);
    trigger = 0;
  }
};

#endif // !__SONAR_H__