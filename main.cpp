#define NOMINMAX // don't let windows screw up min()

#include "sound.h"
#include "asio_helper.h"

#include <algorithm> // for std::min

static Sound<float> snd_;
static ASIOHelper asio_;

void MyAudioCallback(int index)
{
  // our location within the sound to play back
  static int buf_idx = 0;

  // go through the buffers, only using outputs
  ASIOBufferInfo*  b = asio_.buffer_infos_  + asio_.input_channels_;
  ASIOChannelInfo* c = asio_.channel_infos_ + asio_.input_channels_;
  for (int i = 0; i < asio_.output_channels_; ++i, ++b, ++c)
  {
    switch (c->type)
    {
    case ASIOSTInt32LSB:
      {
        int32_t* int32_p = static_cast<int32_t*>(b->buffers[index]);
        for (int j = 0; j < asio_.buffer_size_; ++j)
        {
          // random data
          //float smp = static_cast<float>(rand()) / RAND_MAX;

          // read from sound file
          // take the corresponding channel index if it exists
          int channel = std::min(static_cast<unsigned int>(j), snd_.data.size() - 1);
          float smp = snd_.data[channel][(buf_idx + j) % snd_.data[channel].size()];
          
          // place the output
          *int32_p++ = static_cast<int32_t>(smp * (smp >= 0 ? INT32_MAX : INT32_MIN));
        }
      }
      break;
    default:
      break;
    }
  }

  // adjust the location within the sound for next output
  // assume that each channel contains the same amount of info
  buf_idx += asio_.buffer_size_;
  buf_idx %= snd_.data[0].size();
}


int main(int argc, char* argv[])
{
  // read a wav file
  snd_.ReadWav("C:/Users/clarkson/Desktop/test.wav");

  // use the callback to play it
  asio_.Init(MyAudioCallback);
}