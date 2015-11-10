#ifndef MATRIX_COMPRESSION_DATA_H
#define MATRIX_COMPRESSION_DATA_H

#include <iostream>
#include <vector>
#include <fftw3.h>
#include "COMPRESSION_DATA.h"
#include "FIELD_3D.h"

using std::vector;

class MATRIX_COMPRESSION_DATA {
  public: 
    MATRIX_COMPRESSION_DATA(); 

    MATRIX_COMPRESSION_DATA(int* dataX, int* dataY, int* dataZ,
        COMPRESSION_DATA* compression_dataX, COMPRESSION_DATA* compression_dataY, COMPRESSION_DATA* compression_dataZ);
    ~MATRIX_COMPRESSION_DATA();

   
    // getters
    
    int* get_dataX() const { return _dataX; }
    int* get_dataY() const { return _dataY; }
    int* get_dataZ() const { return _dataZ; }

    COMPRESSION_DATA* get_compression_dataX() { return &_compression_dataX; }
    COMPRESSION_DATA* get_compression_dataY() { return &_compression_dataY; }
    COMPRESSION_DATA* get_compression_dataZ() { return &_compression_dataZ; }

    const fftw_plan& get_plan() { return _dct_plan; }
    double* get_dct_in() { return _dct_in; }

    vector<FIELD_3D>* get_cachedBlocksX() { return &_cachedBlocksX; }
    vector<FIELD_3D>* get_cachedBlocksY() { return &_cachedBlocksY; }
    vector<FIELD_3D>* get_cachedBlocksZ() { return &_cachedBlocksZ; }

    int get_cachedBlockNumber() const { return _cachedBlockNumber; }
    
    // setters
    
    void set_dataX(int* dataX) { _dataX = dataX; }
    void set_dataY(int* dataY) { _dataY = dataY; }
    void set_dataZ(int* dataZ) { _dataZ = dataZ; }

    void set_cachedBlocksX(const vector<FIELD_3D>& cachedBlocksX) { _cachedBlocksX = cachedBlocksX; }
    void set_cachedBlocksY(const vector<FIELD_3D>& cachedBlocksY) { _cachedBlocksY = cachedBlocksY; }
    void set_cachedBlocksZ(const vector<FIELD_3D>& cachedBlocksZ) { _cachedBlocksZ = cachedBlocksZ; }

    void set_cachedBlockNumber(int cachedBlockNumber) { _cachedBlockNumber = cachedBlockNumber; }

    void set_planType(int planType) { _planType = planType; }
   
    // initializations.
    // don't call this until _numCols has been set!
    void init_cache()
    {
      cout << "Calling init_cache()! " << endl;

      // initialize cached block number to nonsense
      _cachedBlockNumber = -1;

      const int xRes = BLOCK_SIZE;
      const int yRes = BLOCK_SIZE;
      const int zRes = BLOCK_SIZE;

      // clunky, but it works
      int numCols = this->_compression_dataX.get_numCols();

      _cachedBlocksX.resize(numCols);
      for (auto itr = _cachedBlocksX.begin(); itr != _cachedBlocksX.end(); ++itr) {
        (*itr).resizeAndWipe(xRes, yRes, zRes);
      }
     
      _cachedBlocksY.resize(numCols);
      for (auto itr = _cachedBlocksY.begin(); itr != _cachedBlocksY.end(); ++itr) {
        (*itr).resizeAndWipe(xRes, yRes, zRes);
      }
      
      _cachedBlocksZ.resize(numCols);
      for (auto itr = _cachedBlocksZ.begin(); itr != _cachedBlocksZ.end(); ++itr) {
        (*itr).resizeAndWipe(xRes, yRes, zRes);
      }

    }


    void dct_setup(int direction)
    {
      cout << "Calling dct_setup(direction)! " << endl;

      const int xRes = BLOCK_SIZE;
      const int yRes = BLOCK_SIZE;
      const int zRes = BLOCK_SIZE;

      _dct_in = (double*) fftw_malloc(xRes * yRes * zRes * sizeof(double));

      if (direction == 1) { // forward transform
         _dct_plan = fftw_plan_r2r_3d(zRes, yRes, xRes, _dct_in, _dct_in, 
             FFTW_REDFT10, FFTW_REDFT10, FFTW_REDFT10, FFTW_MEASURE); 
      }

      else { // direction == -1; backward transform
         _dct_plan = fftw_plan_r2r_3d(zRes, yRes, xRes, _dct_in, _dct_in, 
      FFTW_REDFT01, FFTW_REDFT01, FFTW_REDFT01, FFTW_MEASURE);
      }
    }

 // more general dct/dst transform initialization, using 'planType'
  void transform_setup(int direction) {
    printf("Calling transform_setup with plantype %i\n", _planType);

    const int xRes = BLOCK_SIZE;
    const int yRes = BLOCK_SIZE;
    const int zRes = BLOCK_SIZE;

    cout << "Using block size: " << BLOCK_SIZE << endl;

    _dct_in = (double*) fftw_malloc(xRes * yRes * zRes * sizeof(double));
    fftw_r2r_kind kind_x, kind_y, kind_z;
    if (direction == 1) { // forward transform
      switch (_planType) {
        case 0 : 
          // REDFT10 stands for real even dft (of type 2), corresponding to DCT 
          // RODFT10 stands for real odd dft (of type 2), corresponding to DST
          kind_x = FFTW_REDFT10;
          kind_y = FFTW_REDFT10;
          kind_z = FFTW_REDFT10;
          break;
        case 1 : 
          kind_x = FFTW_REDFT10;
          kind_y = FFTW_REDFT10;
          kind_z = FFTW_RODFT10;
          break;
        case 2 :
          kind_x = FFTW_REDFT10;
          kind_y = FFTW_RODFT10;
          kind_z = FFTW_REDFT10;
          break;
        case 3 :
          kind_x = FFTW_REDFT10;
          kind_y = FFTW_RODFT10;
          kind_z = FFTW_RODFT10;
          break;
        case 4 :
          kind_x = FFTW_RODFT10;
          kind_y = FFTW_REDFT10;
          kind_z = FFTW_REDFT10;
          break;
        case 5 :
          kind_x = FFTW_RODFT10;
          kind_y = FFTW_REDFT10;
          kind_z = FFTW_RODFT10;
          break;
        case 6 :
          kind_x = FFTW_RODFT10;
          kind_y = FFTW_RODFT10;
          kind_z = FFTW_REDFT10;
          break;
        case 7 :
          kind_x = FFTW_RODFT10;
          kind_y = FFTW_RODFT10;
          kind_z = FFTW_RODFT10;
          break;
        default :
          printf("Invalid plan type (must be integer from 0-7 inclusive)\n");
      }
    }
  else { // inverse transforms
   switch (_planType) {
      case 0 : 
        kind_x = FFTW_REDFT01;
        kind_y = FFTW_REDFT01;
        kind_z = FFTW_REDFT01;
        break;
      case 1 : 
        kind_x = FFTW_REDFT01;
        kind_y = FFTW_REDFT01;
        kind_z = FFTW_RODFT01;
        break;
      case 2 :
        kind_x = FFTW_REDFT01;
        kind_y = FFTW_RODFT01;
        kind_z = FFTW_REDFT01;
        break;
      case 3 :
        kind_x = FFTW_REDFT01;
        kind_y = FFTW_RODFT01;
        kind_z = FFTW_RODFT01;
        break;
      case 4 :
        kind_x = FFTW_RODFT01;
        kind_y = FFTW_REDFT01;
        kind_z = FFTW_REDFT01;
        break;
      case 5 :
        kind_x = FFTW_RODFT01;
        kind_y = FFTW_REDFT01;
        kind_z = FFTW_RODFT01;
        break;
      case 6 :
        kind_x = FFTW_RODFT01;
        kind_y = FFTW_RODFT01;
        kind_z = FFTW_REDFT01;
        break;
      case 7 :
        kind_x = FFTW_RODFT01;
        kind_y = FFTW_RODFT01;
        kind_z = FFTW_RODFT01;
        break;
      default :
        printf("Invalid plan type (must be integer from 0-7 inclusive)\n");
    }
  }
  // 'in' appears twice since it is in-place
  // r2r means 'real-to-real'
  _dct_plan = fftw_plan_r2r_3d(zRes, yRes, xRes, _dct_in, _dct_in, kind_z, kind_y, kind_x, FFTW_MEASURE);
  }
   void dct_cleanup() {
     fftw_destroy_plan(_dct_plan);
     fftw_free(_dct_in);
     fftw_cleanup();
   } 


  private:
    int* _dataX;
    int* _dataY;
    int* _dataZ;


    COMPRESSION_DATA _compression_dataX;
    COMPRESSION_DATA _compression_dataY;
    COMPRESSION_DATA _compression_dataZ;

    vector<FIELD_3D> _cachedBlocksX;
    vector<FIELD_3D> _cachedBlocksY;
    vector<FIELD_3D> _cachedBlocksZ;

    int _cachedBlockNumber;

    double* _dct_in;
    int _planType;
    fftw_plan _dct_plan;
};

#endif
