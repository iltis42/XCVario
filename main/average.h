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
			num_sample = num_sample % length;
			return (Total)(total / length );
		}
		else{
			samples[num_sample] = sample;
			num_sample++;
			if( num_sample == length ){
				full = true;
				num_sample = 0;
				return (Total)(total / length);
			}else
				return (Total)(total / num_sample);
		}
	}
	void setLength( int len ){ if( len <=N ){ length = len; } };

private:
	T     samples[N]{0};
	int   length{N};
	int   num_sample{0};
	bool  full{false};
	Total total{0};
};

