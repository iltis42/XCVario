// World Magnetic Model from http://www.ngdc.noaa.gov/geomag/WMM/

#pragma once

// #define WMM_Test 1

class WMM_Model
{
public:
  WMM_Model() = delete;
  WMM_Model(int maxdeg, float epoch, float* wmmcof);
  bool geomag(float alt, float glat, float glon, float time, float *bx, float *by, float *bz, float *bh, float *dec, float *dip, float *ti, float *gv);

  // a unit test
  static void geomag_test();

private:
  int maxord;
  float epoch;
  float sp[13], cp[13], pp[13], dp[13][13];
  float c[13][13], cd[13][13], tc[13][13], snorm[169], fn[13], fm[13], k[13][13];
};

