
template <int N, typename T=int, typename Total=int >
class Average
{
  public:
    T operator()(T sample)
    {
        total_ += sample;
        if (num_samples_ < N)
            samples_[num_samples_++] = sample;
        else
        {
            T& oldest = samples_[num_samples_++ % N];
            total_ -= oldest;
            oldest = sample;
        }
        return (Total)(total_ / N);
    }

  private:
    T samples_[N];
    int num_samples_{0};
    Total total_{0};
};

