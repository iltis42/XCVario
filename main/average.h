#pragma once

template <int N, typename T=int, typename Total=int >
class Average
{
public:
	T operator()(T sample)
	{
		total += sample;
		if( full ){
			T oldest = samples[num_sample];
			samples[num_sample] = sample;
			total -= oldest;
			num_sample++;
			num_sample = num_sample % N;
			return (Total)(total / N );
		}
		else{
			samples[num_sample] = sample;
			num_sample++;
			if( num_sample == N ){
				full = true;
				num_sample = 0;
				return (Total)(total / N);
			}else
				return (Total)(total / num_sample);
		}
	}

private:
	T samples[N]{0};
	uint8_t  num_sample{0};
	uint8_t  full{false};
	Total total{0};
};

