
#include <windows.h>
#include "SnakeDLL.h"

//Definição da variável global
HANDLE hMemoria;
HANDLE hSemMemoria;
HANDLE hEventoMemoria;
HANDLE hFicheiro;
MemGeral *vistaPartilhaGeral;

void inserePedido(Pedido param);
void apagaPedido();

int preparaMemoriaPartilhada(void) {

	hFicheiro = CreateFile(FILE_MAP_NAME, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	hMemoria = CreateFileMapping(hFicheiro, NULL, PAGE_READWRITE, 0, SIZE_MEM_GERAL, NOME_MEM_GERAL);

	vistaPartilhaGeral = (MemGeral*)MapViewOfFile(hMemoria, FILE_MAP_ALL_ACCESS, 0, 0, SIZE_MEM_GERAL);

	hEventoMemoria = CreateEvent(NULL, TRUE, FALSE, EVNT_MEM_GERAL);
	hSemMemoria = CreateSemaphore(NULL, MAXCLIENTES, MAXCLIENTES, SEM_MEM_GERAL);

	if (hMemoria == NULL || hEventoMemoria == NULL || hSemMemoria == NULL) {
		_tprintf(TEXT("[Erro] Criação de objectos do Windows(%d)\n"), GetLastError());
		return -1;
	}
	return 1;
}

void esperaPorActualizacao(void) {
	WaitForSingleObject(hEventoMemoria, INFINITE);
}

/*
void leMemoriaPartilhada(MemGeral* param) {

WaitForSingleObject(hSemMemoria, INFINITE);

param->estadoJogo = vistaPartilhaGeral->estadoJogo;
param->mensagem.codigoMsg = vistaPartilhaGeral->mensagem.codigoMsg;
_tcscpy_s(param->mensagem.username, SIZE_USERNAME, vistaPartilhaGeral->mensagem.username);
param->config.C = vistaPartilhaGeral->config.C;
param->config.L = vistaPartilhaGeral->config.L;

ReleaseSemaphore(hSemMemoria, 1, NULL);
}*/

void fechaMemoriaPartilhada(void) {
	CloseHandle(hMemoria);
	CloseHandle(hSemMemoria);
	CloseHandle(hEventoMemoria);
	UnmapViewOfFile(vistaPartilhaGeral);
}

void getMapa(MemGeral *param) {
	WaitForSingleObject(hSemMemoria, INFINITE);
	for (int i = 0; i < vistaPartilhaGeral->linhas; i++) {
		for (int j = 0; j < vistaPartilhaGeral->colunas; j++) {
			param->mapa[i][j] = vistaPartilhaGeral->mapa[i][j];
		}
	}
	ReleaseSemaphore(hSemMemoria, 1, NULL);
}

int pede_CriaJogo(ConfigInicial param, int pid) {
	Pedido aux;
	aux.config = param;
	aux.pid = pid;
	aux.codigoPedido = CRIARJOGO;
	for (int i = 0; i < MAXCLIENTES; i++) {
		WaitForSingleObject(hSemMemoria, INFINITE);
	}

	inserePedido(aux);

	SetEvent(hEventoMemoria);
	ResetEvent(hEventoMemoria);
	ReleaseSemaphore(hSemMemoria, MAXCLIENTES, NULL);
	return 1;
}

int pede_IniciaJogo(int pid) {
	Pedido aux;
	aux.pid = pid;
	aux.codigoPedido = INICIARJOGO;
	for (int i = 0; i < MAXCLIENTES; i++) {
		WaitForSingleObject(hSemMemoria, INFINITE);
	}

	inserePedido(aux);

	SetEvent(hEventoMemoria);
	ResetEvent(hEventoMemoria);
	ReleaseSemaphore(hSemMemoria, MAXCLIENTES, NULL);
	return 1;
}

int pede_AssociaJogo(int Pid, TCHAR username[SIZE_USERNAME], int codigoPedido) {
	Pedido aux;
	aux.codigoPedido = codigoPedido;
	aux.pid = Pid;
	_tcscpy_s(aux.username, SIZE_USERNAME, username);

	for (int i = 0; i < MAXCLIENTES; i++) {
		WaitForSingleObject(hSemMemoria, INFINITE);
	}

	inserePedido(aux);

	SetEvent(hEventoMemoria);
	ResetEvent(hEventoMemoria);
	ReleaseSemaphore(hSemMemoria, MAXCLIENTES, NULL);
	return 1;
}


void inserePedido(Pedido param) {
	vistaPartilhaGeral->fila.pedidos[vistaPartilhaGeral->fila.tras].pid = param.pid;
	vistaPartilhaGeral->fila.pedidos[vistaPartilhaGeral->fila.tras].codigoPedido = param.codigoPedido;
	vistaPartilhaGeral->fila.pedidos[vistaPartilhaGeral->fila.tras].config = param.config;
	_tcscpy_s(vistaPartilhaGeral->fila.pedidos[vistaPartilhaGeral->fila.tras].username, SIZE_USERNAME, param.username);
	for (int i = 0; i < NUMTIPOOBJECTOS; i++)
		vistaPartilhaGeral->fila.pedidos[vistaPartilhaGeral->fila.tras].objectos[i] = param.objectos[i];
	vistaPartilhaGeral->fila.tras++;
	//chegou ao fim da fila temos de voltar a por desde o inicio da fila
	if (vistaPartilhaGeral->fila.tras == MAX_PEDIDOS - 1) {
		vistaPartilhaGeral->fila.tras = 0;
	}
	//se a frente estiver a apontar para -1 quer dizer que a fila estava vazia e temos de por a apontar para a primeira posição
	if (vistaPartilhaGeral->fila.frente == -1) {
		vistaPartilhaGeral->fila.frente = 0;
	}
}

void apagaPedido() {
	//se a frente for maior que a parte de tras temos a fila vazia e podemos por os dois indices para o inicio
	if (vistaPartilhaGeral->fila.frente > vistaPartilhaGeral->fila.tras) {
		vistaPartilhaGeral->fila.frente = -1;
		vistaPartilhaGeral->fila.tras = 0;
	}
	vistaPartilhaGeral->fila.frente++;
}

void mudaDirecao(int direcao, int Pid) {
	Pedido aux;
	aux.pid = Pid;
	aux.codigoPedido = direcao;
	for (int i = 0; i < MAXCLIENTES; i++) {
		WaitForSingleObject(hSemMemoria, INFINITE);
	}

	inserePedido(aux);

	ReleaseSemaphore(hSemMemoria, MAXCLIENTES, NULL);
}