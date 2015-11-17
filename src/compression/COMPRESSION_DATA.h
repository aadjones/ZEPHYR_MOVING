#ifndef COMPRESSION_DATA_H
#define COMPRESSION_DATA_H

#include "EIGEN.h"
#include <iostream>
#include <fftw3.h>
#include "FIELD_3D.h"
#include "INTEGER_FIELD_3D.h"

// pushed to SETTINGS.h
//#define BLOCK_SIZE 8
//#define BLOCK_SIZE 16

using std::cout;
using std::endl;


class COMPRESSION_DATA {
  public:
    COMPRESSION_DATA();
    COMPRESSION_DATA(const VEC3I& dims, int numCols, int nBits, double percent);
    ~COMPRESSION_DATA();


    // getters
    const VEC3I& get_dims() const { return _dims; }
    const VEC3I& get_paddedDims() const { return _paddedDims; }
    int get_numCols() const { return _numCols; }
    int get_numBlocks() const { return _numBlocks; }
    int get_currBlockNum() const { return _currBlockNum; }
    double get_percent() const { return _percent; }
    int get_nBits() const { return _nBits; } 
    int get_maxIterations() const { return _maxIterations; } 
    bool get_debug() const { return _debug; }
    const VectorXi& get_blockLengths() const { return _blockLengths; }
    const VectorXi& get_blockIndices() const { return _blockIndices; }
    
    // adding matrix versions within compression data
    MatrixXi* get_blockLengthsMatrix() { return &(_blockLengthsMatrix); }
    MatrixXi* get_blockIndicesMatrix() { return &(_blockIndicesMatrix); }

    // modified get_sList and gammaList to break const-ness
    VectorXd* get_sList() { return &(_sList); }
    VectorXd* get_gammaList() { return &(_gammaList); }

    MatrixXd* get_sListMatrix() { return &(_sListMatrix); }
    MatrixXd* get_gammaListMatrix() { return &(_gammaListMatrix); }

    vector<Vector3d>* get_singularList() { return &(_singularList); }
    vector<Matrix3d>* get_vList() { return &(_vList); }
    
    VECTOR* get_singularValues() { return &(_singularValues); }

    const FIELD_3D& get_dampingArray() const { return _dampingArray; }
    vector<FIELD_3D>* get_dampingArrayList() { return &(_dampingArrayList); }
    bool get_arrayListBuilt() const { return _arrayListBuilt; }
    const INTEGER_FIELD_3D& get_zigzagArray() const { return _zigzagArray; }
    const INTEGER_FIELD_3D& get_reverseZigzag() const { return _reverseZigzag; }
    double* get_dct_in() const { return _dct_in; }
    double* get_dct_out() const { return _dct_out; }
    fftw_plan get_dct_plan() const { return _dct_plan; }

    // setters
    void set_dims(const VEC3I& dims) { _dims = dims; }
    void set_paddedDims(const VEC3I& paddedDims) { _paddedDims = paddedDims; }
    void set_numCols(int numCols) { _numCols = numCols; }
    void set_numBlocks(int numBlocks) { _numBlocks = numBlocks; }
    void set_currBlockNum(int currBlockNum) { _currBlockNum = currBlockNum; }
    void set_percent(double percent) { _percent = percent; }
    void set_nBits(int nBits) { _nBits = nBits; }
    void set_maxIterations(int maxIterations) { _maxIterations = maxIterations; }
    void set_debug(bool debug) { _debug = debug; }

    void set_blockLengths(const VectorXi& blockLengths) { 
      int length = blockLengths.size();
      assert(length == _numBlocks);
      _blockLengths = blockLengths; 
    }

    void set_blockIndices(const VectorXi& blockIndices) { 
      int length = blockIndices.size();
      assert(length == _numBlocks);
      _blockIndices = blockIndices;
    }

    void set_sList(const VectorXd& sList) { 
      int length = sList.size();
      assert(length == _numBlocks);
      _sList = sList;
    
    }
    
    void set_vList(const vector<Matrix3d>& vList) {
      int length = vList.size();
      assert(length == _numCols);
      _vList = vList;
    }

    // compute and set damping array

    void set_dampingArray() {
      int uRes = BLOCK_SIZE;
      int vRes = BLOCK_SIZE;
      int wRes = BLOCK_SIZE;
      FIELD_3D damp(uRes, vRes, wRes);

      for (int w = 0; w < wRes; w++) {
        for (int v = 0; v < vRes; v++) {
          for (int u = 0; u < uRes; u++) {
            damp(u, v, w) = 1 + u + v + w;
          }
        }
      }
      _dampingArray = damp;
      _arrayListBuilt = true;
    }
  
  void set_dampingArrayList() {
    // choose the number of different discrete gamma values to clamp to
    // approximately nBits (32) * 4 is a first test
    int numGammas = 129; 
    _dampingArrayList.resize(numGammas);
    for (int i = 0; i < numGammas; i++) {
      _dampingArrayList[i] = _dampingArray;
      double pow = i / 4.0;
      _dampingArrayList[i].toPower(pow);
    }
  }

  // read in the singular values from a binary file
  void set_singularValues(const char* filename) {
    _singularValues.read(filename);
  }


    
  void set_zigzagArray() {
    TIMER functionTimer(__FUNCTION__);

    int xRes = BLOCK_SIZE;
    int yRes = BLOCK_SIZE; 
    int zRes = BLOCK_SIZE; 
    INTEGER_FIELD_3D zigzagArray(xRes, yRes, zRes);
    int sum;
    int i = 0;
    for (sum = 0; sum < xRes + yRes + zRes; sum++) {
      for (int z = 0; z < zRes; z++) {
        for (int y = 0; y < yRes; y++) {
          for (int x = 0; x < xRes; x++) {
            if (x + y + z == sum) {
              zigzagArray(x, y, z) = i;
              i++;
            }
          }
        }
      }
    }
    _zigzagArray = zigzagArray;

    // cache the reverse too
    _reverseZigzag = _zigzagArray;
    for (int x = 0; x < zigzagArray.totalCells(); x++)
      _reverseZigzag[zigzagArray[x]] = x;
  }

  void set_planType(int planType) { _planType = planType; }

  void dct_setup(int direction) {
    const int xRes = BLOCK_SIZE;
    const int yRes = BLOCK_SIZE;
    const int zRes = BLOCK_SIZE;

    cout << "Using block size: " << BLOCK_SIZE << endl;

    _dct_in = (double*) fftw_malloc(xRes * yRes * zRes * sizeof(double));
    _dct_out = (double*) fftw_malloc(xRes * yRes * zRes * sizeof(double));

    if (direction == 1) { // forward transform
       _dct_plan = fftw_plan_r2r_3d(zRes, yRes, xRes, _dct_in, _dct_out, 
           FFTW_REDFT10, FFTW_REDFT10, FFTW_REDFT10, FFTW_MEASURE); 
    }

    else { // direction == -1; backward transform
       _dct_plan = fftw_plan_r2r_3d(zRes, yRes, xRes, _dct_in, _dct_out, 
    FFTW_REDFT01, FFTW_REDFT01, FFTW_REDFT01, FFTW_MEASURE);
    }
  }
  
  // more general dct/dst transform initialization, using 'planType'
  void transform_setup(int direction) {
    const int xRes = BLOCK_SIZE;
    const int yRes = BLOCK_SIZE;
    const int zRes = BLOCK_SIZE;

    cout << "Using block size: " << BLOCK_SIZE << endl;

    _dct_in = (double*) fftw_malloc(xRes * yRes * zRes * sizeof(double));
    _dct_out = (double*) fftw_malloc(xRes * yRes * zRes * sizeof(double));
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
  _dct_plan = fftw_plan_r2r_3d(zRes, yRes, xRes, _dct_in, _dct_out, kind_z, kind_y, kind_x, FFTW_MEASURE);
  }

 void dct_cleanup() {
   fftw_destroy_plan(_dct_plan);
   fftw_free(_dct_in);
   fftw_free(_dct_out);
   fftw_cleanup();
 }

  private:
    VEC3I _dims;
    VEC3I _paddedDims;
    int _numCols;
    int _numBlocks;
    int _currBlockNum;
    int _maxIterations;
    bool _debug;
    double _nBits;
    double _percent;

    VectorXi _blockLengths;
    VectorXi _blockIndices;
    VectorXd _sList;
    VectorXd _gammaList;

    MatrixXi _blockLengthsMatrix;
    MatrixXi _blockIndicesMatrix;
    MatrixXd _sListMatrix;
    MatrixXd _gammaListMatrix;

    vector<Matrix3d> _vList;
    vector<Vector3d> _singularList;
    VECTOR _singularValues;

    FIELD_3D _dampingArray;
    INTEGER_FIELD_3D _zigzagArray;
    INTEGER_FIELD_3D _reverseZigzag;
    
    vector<FIELD_3D> _dampingArrayList;
    bool _arrayListBuilt;
    double* _dct_in;
    double* _dct_out;
    int _planType;
    fftw_plan _dct_plan;
};

#endif

