#include <stdlib.h>
#include "hash.h"

// Struct de los nodos del hash
struct nodo_hash {
	char *clave;
	void *valor;
};

struct hash {
	size_t cantidad;
	size_t tamanio;
	hash_destruir_dato_t destruir_dato;
	lista_t** tabla;
};

struct hash_iter{
	const hash_t *hash; 
	size_t pos_vect;
	lista_iter_t *iter_lista;
};

/* FUNCIONES AUXILIARES */

// Crea un nuevo nodo del hash
nodo_hash_t* _nodo_hash_crear(char *clave, void *dato) {
	nodo_hash_t* nodo_hash = NULL;
	if (!(nodo_hash = malloc(sizeof(nodo_hash_t)))) return NULL;
	nodo_hash->clave = clave;
	nodo_hash->valor = dato;

	return nodo_hash;
}

// Devuelve el factor de carga del hash
float factor_de_carga(hash_t *hash) {
	return ((float) (hash->cantidad / hash->tamanio));
}

// Función hash 1
size_t DJBHash(char* clave, size_t tam) {
	unsigned int hash = 5381;
	unsigned int i = 0;

	for(i = 0; i < tam; clave++, i++) {
		hash = ((hash << 5) + hash) + (*clave);
	}

	return hash;
}

// Función hash 2
size_t fhash(const char* clave, size_t tam) {
	unsigned int hash = 0;
	unsigned int i = 0;
	while (clave[i] != '\0') {
		hash += ((int) clave[i] + 13);
		i++;
	}
	return hash%tam;
}

/* FUNCIONES DEL HASH */

hash_t *hash_crear(hash_destruir_dato_t destruir_dato) {
	
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato) {
	
}

void *hash_borrar(hash_t *hash, const char *clave) {
	
}

void *hash_obtener(const hash_t *hash, const char *clave) {
	
}

bool hash_pertenece(const hash_t *hash, const char *clave) {
	
}

size_t hash_cantidad(const hash_t *hash) {
	
}

void hash_destruir(hash_t *hash) {
	
}

hash_iter_t *hash_iter_crear(const hash_t *hash) {
	
}

bool hash_iter_avanzar(hash_iter_t *iter) {
	
}

const char *hash_iter_ver_actual(const hash_iter_t *iter) {
	
}

bool hash_iter_al_final(const hash_iter_t *iter) {
	
}

void hash_iter_destruir(hash_iter_t* iter) {
	
}