
#include <windows.h>
#include "SnakeDLL.h"

//Definição da variável global
HANDLE hMemoria;
HANDLE hSemaforoMapa;
HANDLE hPodeLerPedido;
HANDLE hPodeEscreverPedido;
HANDLE hEventoMapa;
HANDLE hFicheiro;
HANDLE hMemResposta;
HANDLE hEventoResposta;
MemGeral *vistaPartilhaGeral;
Resposta *vistaResposta;

void inserePedido(Pedido param);


int preparaMemoriaPartilhada(void) {

	//hFicheiro = CreateFile(NOME_FILE_MAP, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	hMemoria = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SIZE_MEM_GERAL, NOME_MEM_GERAL);

	vistaPartilhaGeral = (MemGeral*)MapViewOfFile(hMemoria, FILE_MAP_ALL_ACCESS, 0, 0, SIZE_MEM_GERAL);

	hEventoMapa = CreateEvent(NULL, TRUE, FALSE, NOME_EVNT_MAPA);
	hSemaforoMapa = CreateSemaphore(NULL, MAXCLIENTES, MAXCLIENTES, NOME_SEM_MAPA);

	hPodeLerPedido = CreateSemaphore(NULL, 0, MAX_PEDIDOS, NOME_SEM_PODELER);
	hPodeEscreverPedido = CreateSemaphore(NULL, MAX_PEDIDOS, MAX_PEDIDOS, NOME_SEM_PODESCRVR);

	if (hMemoria == NULL || hEventoMapa == NULL || hSemaforoMapa == NULL || hPodeLerPedido == NULL || hPodeEscreverPedido == NULL) {
		_tprintf(TEXT("[Erro] Criação de objectos do Windows(%d)\n"), GetLastError());
		return -1;
	}

	vistaPartilhaGeral->fila.frente = 0;
	vistaPartilhaGeral->fila.tras = 0;
	return 1;
}

int preparaMemoriaPartilhadaResposta(int pid) {
	TCHAR aux[TAM_BUFFER];
	TCHAR aux2[TAM_BUFFER];

	//concatenar pid com nome da memoria para ficar com um nome unico
	_stprintf_s(aux, TAM_BUFFER, NOME_MEM_RESPOSTA, pid);

	hMemResposta = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(Resposta), aux);

	vistaResposta = (Resposta*)MapViewOfFile(hMemResposta, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(Resposta));

	//concatenar pid com nome do evento para ficar com um nome unico
	_stprintf_s(aux2, TAM_BUFFER, NOME_EVNT_RESPOSTA, pid);
	hEventoResposta = CreateEvent(NULL, TRUE, FALSE, aux2);

	if (hMemResposta == NULL || hEventoResposta == NULL) {
		_tprintf(TEXT("[Erro] Criação de objectos do Windows(%d)\n"), GetLastError());
		return -1;
	}
	return 1;
}

void esperaPorActualizacaoMapa(void) {
	WaitForSingleObject(hEventoMapa, INFINITE);
}

void fechaMemoriaPartilhada(void) {
	CloseHandle(hMemoria);
	CloseHandle(hSemaforoMapa);
	CloseHandle(hEventoMapa);
	UnmapViewOfFile(vistaPartilhaGeral);
}

void getMapa(int mapa[MAX_LINHAS][MAX_COLUNAS]) {
	WaitForSingleObject(hSemaforoMapa, INFINITE);
	for (int i = 0; i < vistaPartilhaGeral->linhas; i++) {
		for (int j = 0; j < vistaPartilhaGeral->colunas; j++) {
			mapa[i][j] = vistaPartilhaGeral->mapa[i][j];
		}
	}
	ReleaseSemaphore(hSemaforoMapa, 1, NULL);
}

void getLimitesMapa(int *linhas, int *colunas) {
	WaitForSingleObject(hSemaforoMapa, INFINITE);
	*linhas=vistaPartilhaGeral->linhas;
	*colunas = vistaPartilhaGeral->colunas;
	ReleaseSemaphore(hSemaforoMapa, 1, NULL);
	
}

int pede_CriaJogo(ConfigInicial param, int pid, TCHAR username[SIZE_USERNAME]) {
	Pedido aux;
	aux.config = param;
	aux.pid = pid;
	aux.codigoPedido = CRIARJOGO;
	_tcscpy_s(aux.username, SIZE_USERNAME, username);
	
	inserePedido(aux);

	return 1;
}

int pede_IniciaJogo(int pid) {
	Pedido aux;
	aux.pid = pid;
	aux.codigoPedido = INICIARJOGO;
	_tcscpy_s(aux.username, SIZE_USERNAME, TEXT(" "));
	
	inserePedido(aux);

	return 1;
}

int pede_RegistarClienteLocal(int pid) {
	Pedido aux;
	aux.pid = pid;
	aux.codigoPedido = REGISTACLIENTELOCAL;
	_tcscpy_s(aux.username, SIZE_USERNAME, TEXT(" "));

	inserePedido(aux);

	return 1;
}

int pede_RegistarClienteRemoto(int pid) {
	Pedido aux;
	aux.pid = pid;
	aux.codigoPedido = REGISTACLIENTEREMTO;
	_tcscpy_s(aux.username, SIZE_USERNAME, TEXT(" "));

	inserePedido(aux);

	return 1;
}

int pede_AssociaJogo(int Pid, TCHAR username[SIZE_USERNAME], int codigoPedido) {
	Pedido aux;
	aux.codigoPedido = codigoPedido;
	aux.pid = Pid;
	_tcscpy_s(aux.username, SIZE_USERNAME, username);

	inserePedido(aux);

	return 1;
}


void mudaDirecao(int direcao, int Pid, int jogador) {
	Pedido aux;
	aux.pid = Pid;
	aux.codigoPedido = direcao;
	aux.jogador = jogador;
	_tcscpy_s(aux.username, SIZE_USERNAME, TEXT(" "));

	inserePedido(aux);
}


void inserePedido(Pedido param) {

	//Espera que haja uma vaga para escrever um pedido
	WaitForSingleObject(hPodeEscreverPedido, INFINITE);

	vistaPartilhaGeral->fila.pedidos[vistaPartilhaGeral->fila.tras].pid = param.pid;
	vistaPartilhaGeral->fila.pedidos[vistaPartilhaGeral->fila.tras].codigoPedido = param.codigoPedido;
	vistaPartilhaGeral->fila.pedidos[vistaPartilhaGeral->fila.tras].config = param.config;
	vistaPartilhaGeral->fila.pedidos[vistaPartilhaGeral->fila.tras].jogador = param.jogador;
	_tcscpy_s(vistaPartilhaGeral->fila.pedidos[vistaPartilhaGeral->fila.tras].username, SIZE_USERNAME, param.username);
	for (int i = 0; i < NUMTIPOOBJECTOS; i++)
		vistaPartilhaGeral->fila.pedidos[vistaPartilhaGeral->fila.tras].objectosConfig[i] = param.objectosConfig[i];
	vistaPartilhaGeral->fila.tras++;
	//chegou ao fim da fila temos de voltar a por desde o inicio da fila
	if (vistaPartilhaGeral->fila.tras == MAX_PEDIDOS) {
		vistaPartilhaGeral->fila.tras = 0;
	}

	//Liberta uma vaga para Ler um pedido
	ReleaseSemaphore(hPodeLerPedido, 1, NULL);
}