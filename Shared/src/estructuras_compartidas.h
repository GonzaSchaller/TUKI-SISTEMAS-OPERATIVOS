#ifndef SRC_ESTRUCTURAS_COMPARTIDAS_H_
#define SRC_ESTRUCTURAS_COMPARTIDAS_H_


typedef struct{
    uint32_t id;
    uint32_t direccion_Base;
    uint32_t tamanio;
}segmento_t;

typedef enum{
   	 EXITOSO,
   	 FALLIDO,
   	 COMPACTAR
} estados_segmentos;

#endif /* SRC_ESTRUCTURAS_COMPARTIDAS_H_ */
