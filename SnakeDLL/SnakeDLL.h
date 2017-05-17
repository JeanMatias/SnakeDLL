//SnakeDLL.h
// O bloco ifdef seguinte é o modo standard de criar macros que tornam a exportação de
// funções e variáveis mais simples. Todos os ficheiros neste projeto DLL são
// compilados com o símbolo DLL_IMP_EXPORTS definido. Este símbolo não deve ser definido
// em nenhum projeto que use a DLL. Desta forma, qualquer outro projeto que inclua este
// este ficheiro irá ver as funções e variáveis DLL_IMP_API como sendo importadas de uma
// DLL.

#include <windows.h>
#include <tchar.h>
#include "TiposConstantes.h"

//Definir uma constante para facilitar a leitura do protótipo da função
//Este .h deve ser incluído no projeto que o vai usar (modo implícito)

//Esta macro é definida pelo sistema caso estejamos na DLL (<DLL_IMP>_EXPORTS definida)
//ou na app (<DLL_IMP>_EXPORTS não definida) onde DLL_IMP é o nome deste projeto
//#ifdef DLL_EXPORTS

//**********************************************************************************
#ifdef DLL_EXPORTS   // VEIO DE -> PROP PROJ -> C++ -> LINHA COMANDO   -->  Está lá o nome do proj + o EXPORT, copiar para aqui
//**********************************************************************************

#define DLL_IMP_API __declspec(dllexport)   // Deve ser acrescentado antes de cada declaração de função ****************
#else										// ou var global a usar fora da DLL, é para exportar		****************
#define DLL_IMP_API __declspec(dllimport)   // Se estiver na DLL faz o de sima, se não estiver faz este
#endif

//Variável global da DLL
extern DLL_IMP_API HANDLE hMemoria;
extern DLL_IMP_API HANDLE hSemMemoria;
extern DLL_IMP_API HANDLE hEventoMemoria;
extern DLL_IMP_API HANDLE hFicheiro;
extern DLL_IMP_API MemGeral *vistaPartilhaGeral;

//Funções a serem exportadas/importadas
DLL_IMP_API int preparaMemoriaPartilhada(void);
DLL_IMP_API void preparaMapaJogo(MemGeral param);
DLL_IMP_API void inicializaMemoriaPartilhada(void);
DLL_IMP_API void esperaPorActualizacao(void);
DLL_IMP_API void leMemoriaPartilhada(MemGeral* param);
DLL_IMP_API void fechaMemoriaPartilhada(void);
DLL_IMP_API int Cria_Jogo(ConfigInicial param, TCHAR username1[SIZE_USERNAME]);
DLL_IMP_API void getMapa(MemGeral *param);
DLL_IMP_API void mudaDirecao(int direcao, int indice);
DLL_IMP_API int AssociaJogo(int numJogadores, TCHAR username1[SIZE_USERNAME], TCHAR username2[SIZE_USERNAME], int *indice);
DLL_IMP_API int IniciaJogo(TCHAR username[SIZE_USERNAME]);

