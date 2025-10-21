#include <iostream>
#include <iomanip>
#include <new>
using namespace std;

template <typename T>
class MatrizBase {
protected:
    int _fil;
    int _col;
public:
    MatrizBase(int f=0,int c=0):_fil(f),_col(c){}
    virtual ~MatrizBase(){}
    virtual void cargarValores() = 0;
    virtual MatrizBase<T>* sumar(const MatrizBase<T>& otra) const = 0;
    virtual void imprimir() const = 0;
    virtual T obtener(int r,int c) const = 0;
    virtual bool poner(int r,int c,T v) = 0;
    int filas() const { return _fil; }
    int cols() const { return _col; }
};

template <typename T>
class MatrizDinamica : public MatrizBase<T> {
private:
    T** _dat;
    void liberar() {
        if(!_dat) return;
        for(int i=0;i<this->_fil;i++) delete[] _dat[i];
        delete[] _dat;
        _dat = nullptr;
    }
    T** alocar(int f,int c) {
        T** m = nullptr;
        try {
            m = new T*[f];
            for(int i=0;i<f;i++){
                m[i] = new T[c];
                for(int j=0;j<c;j++) m[i][j] = T();
            }
        } catch(bad_alloc&) {
            if(m){
                for(int i=0;i<f;i++) if(m[i]) delete[] m[i];
                delete[] m;
            }
            return nullptr;
        }
        return m;
    }
public:
    MatrizDinamica(int f=0,int c=0):MatrizBase<T>(f,c),_dat(nullptr){
        if(f>0 && c>0){
            _dat = alocar(f,c);
            if(!_dat) throw bad_alloc();
        }
    }
    MatrizDinamica(const MatrizDinamica<T>& o):MatrizBase<T>(o._fil,o._col),_dat(nullptr){
        if(o._fil>0 && o._col>0){
            _dat = alocar(o._fil,o._col);
            if(!_dat) throw bad_alloc();
            for(int i=0;i<this->_fil;i++) for(int j=0;j<this->_col;j++) _dat[i][j]=o._dat[i][j];
        }
    }
    MatrizDinamica(MatrizDinamica<T>&& o) noexcept : MatrizBase<T>(o._fil,o._col),_dat(o._dat){
        o._dat = nullptr;
        o._fil = 0; o._col = 0;
    }
    MatrizDinamica<T>& operator=(const MatrizDinamica<T>& o){
        if(this==&o) return *this;
        T** tmp = nullptr;
        if(o._fil>0 && o._col>0){
            tmp = alocar(o._fil,o._col);
            if(!tmp) throw bad_alloc();
            for(int i=0;i<o._fil;i++) for(int j=0;j<o._col;j++) tmp[i][j]=o._dat[i][j];
        }
        liberar();
        _dat = tmp;
        this->_fil = o._fil; this->_col = o._col;
        return *this;
    }
    MatrizDinamica<T>& operator=(MatrizDinamica<T>&& o) noexcept {
        if(this==&o) return *this;
        liberar();
        _dat = o._dat;
        this->_fil = o._fil; this->_col = o._col;
        o._dat = nullptr; o._fil = 0; o._col = 0;
        return *this;
    }
    ~MatrizDinamica(){ liberar(); }

    void cargarValores(){
        for(int i=0;i<this->_fil;i++){
            for(int j=0;j<this->_col;j++){
                T v;
                cout<<"ingresar valor ["<<i<<"]["<<j<<"]: ";
                cin>>v;
                _dat[i][j]=v;
            }
        }
    }

    MatrizBase<T>* sumar(const MatrizBase<T>& otra) const {
        if(this->_fil!=otra.filas() || this->_col!=otra.cols()) return nullptr;
        MatrizDinamica<T>* res = new MatrizDinamica<T>(this->_fil,this->_col);
        for(int i=0;i<this->_fil;i++) for(int j=0;j<this->_col;j++){
            T v1 = this->obtener(i,j);
            T v2 = otra.obtener(i,j);
            res->poner(i,j, v1 + v2);
        }
        return res;
    }

    void imprimir() const {
        cout<<fixed<<setprecision(4);
        for(int i=0;i<this->_fil;i++){
            cout<<"| ";
            for(int j=0;j<this->_col;j++){
                cout<<setw(8)<<_dat[i][j]<<" ";
            }
            cout<<"|\n";
        }
    }

    T obtener(int r,int c) const {
        if(r<0||r>=this->_fil||c<0||c>=this->_col) return T();
        return _dat[r][c];
    }

    bool poner(int r,int c,T v){
        if(r<0||r>=this->_fil||c<0||c>=this->_col) return false;
        _dat[r][c]=v;
        return true;
    }
};

template <typename T, int M, int N>
class MatrizEstatica : public MatrizBase<T> {
private:
    T _dat[M][N];
public:
    MatrizEstatica():MatrizBase<T>(M,N){
        for(int i=0;i<M;i++) for(int j=0;j<N;j++) _dat[i][j]=T();
    }
    void cargarValores(){
        for(int i=0;i<M;i++) for(int j=0;j<N;j++){
            T v;
            cout<<"ingresar valor ["<<i<<"]["<<j<<"]: ";
            cin>>v;
            _dat[i][j]=v;
        }
    }
    MatrizBase<T>* sumar(const MatrizBase<T>& otra) const {
        if(this->_fil!=otra.filas()||this->_col!=otra.cols()) return nullptr;
        MatrizEstatica<T,M,N>* res = new MatrizEstatica<T,M,N>();
        for(int i=0;i<M;i++) for(int j=0;j<N;j++){
            T v1 = _dat[i][j];
            T v2 = otra.obtener(i,j);
            res->poner(i,j, v1 + v2);
        }
        return res;
    }
    void imprimir() const {
        cout<<fixed<<setprecision(4);
        for(int i=0;i<M;i++){
            cout<<"| ";
            for(int j=0;j<N;j++) cout<<setw(8)<<_dat[i][j]<<" ";
            cout<<"|\n";
        }
    }
    T obtener(int r,int c) const {
        if(r<0||r>=M||c<0||c>=N) return T();
        return _dat[r][c];
    }
    bool poner(int r,int c,T v){
        if(r<0||r>=M||c<0||c>=N) return false;
        _dat[r][c]=v;
        return true;
    }
};

template <typename T>
MatrizBase<T>* operator+(const MatrizBase<T>& a,const MatrizBase<T>& b){
    return a.sumar(b);
}

int main(){
    cout<<fixed<<setprecision(4);
    MatrizBase<float>* a = new MatrizDinamica<float>(3,2);
    a->poner(0,0,1.5f); a->poner(0,1,2.0f);
    a->poner(1,0,0.0f); a->poner(1,1,1.0f);
    a->poner(2,0,4.5f); a->poner(2,1,3.0f);
    cout<<"A (din float):\n"; a->imprimir();

    MatrizBase<float>* b = new MatrizEstatica<float,3,2>();
    b->poner(0,0,0.5f); b->poner(0,1,1.0f);
    b->poner(1,0,2.0f); b->poner(1,1,3.0f);
    b->poner(2,0,1.0f); b->poner(2,1,1.0f);
    cout<<"\nB (est float):\n"; b->imprimir();

    MatrizBase<float>* c = *a + *b;
    if(c){
        cout<<"\nC = A + B (float):\n"; c->imprimir();
        delete c;
    } else cout<<"\nError suma float\n";

    delete a; delete b;

    MatrizBase<int>* p = new MatrizDinamica<int>(2,3);
    p->poner(0,0,1); p->poner(0,1,2); p->poner(0,2,3);
    p->poner(1,0,4); p->poner(1,1,5); p->poner(1,2,6);
    cout<<"\nP (din int):\n"; p->imprimir();

    MatrizBase<int>* q = new MatrizEstatica<int,2,3>();
    q->poner(0,0,10); q->poner(0,1,20); q->poner(0,2,30);
    q->poner(1,0,40); q->poner(1,1,50); q->poner(1,2,60);
    cout<<"\nQ (est int):\n"; q->imprimir();

    MatrizBase<int>* r = *p + *q;
    if(r){
        cout<<"\nR = P + Q (int):\n"; r->imprimir();
        delete r;
    } else cout<<"\nError suma int\n";

    delete p; delete q;

    return 0;
}
