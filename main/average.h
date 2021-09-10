template <int N, class input_t = int, class sum_t = int>
class Average {
  public:
    input_t operator()(input_t input) {
        sum -= previousInputs[index];
        sum += input;
        previousInputs[index] = input;
        if (++index == customN)
            index = 0;
        return (sum  / customN);
    }

    void setN( int n ){
    	customN = n;
    }

  private:
    uint8_t index             = 0;
    uint8_t customN = N;
    input_t previousInputs[N] = {};
    sum_t sum                 = 0;
};
