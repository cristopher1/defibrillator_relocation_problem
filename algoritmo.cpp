#include "algoritmo.h"

/*implementacion de los metodos de la clase ResultadoCT*/

ResultadoCT::ResultadoCT()
{
  this->cobertura = 0;
  this->eventosCubiertos = std::set<coordenadas>();
}

ResultadoCT::ResultadoCT(const unsigned long cobertura, const std::set<coordenadas> eventosCubiertos)
{
  this->cobertura = cobertura;
  this->eventosCubiertos = eventosCubiertos;
}

unsigned long ResultadoCT::getCobertura()
{
  return this->cobertura;
}

std::set<coordenadas> ResultadoCT::getEventosCubiertos()
{
  return this->eventosCubiertos;
}

/*implementacion de los metodos de la clase ResultadoFEv*/

ResultadoFEv::ResultadoFEv()
{
  this->costo = 0.0;
  this->cobertura = 0;
  this->eventosCubiertos = std::set<coordenadas>();
}

ResultadoFEv::ResultadoFEv(const float costo, const unsigned long cobertura,
                           const std::set<coordenadas> eventosCubiertos)
{
  this->costo = costo;
  this->cobertura = cobertura;
  this->eventosCubiertos = eventosCubiertos;
}

float ResultadoFEv::getCosto()
{
  return this->costo;
}

unsigned long ResultadoFEv::getCobertura()
{
  return this->cobertura;
}

std::set<coordenadas> ResultadoFEv::getEventosCubiertos()
{
  return this->eventosCubiertos;
}

/*implementacion de los metodos de la clase ResultadoHCMM*/

ResultadoHCMM::ResultadoHCMM()
{
  this->resultado = posicion();
  this->costo = 0.0;
  this->cobertura = 0;
}

ResultadoHCMM::ResultadoHCMM(const posicion resultado, const float costo, const unsigned long cobertura)
{
  this->resultado = resultado;
  this->costo = costo;
  this->cobertura = cobertura;
}

posicion ResultadoHCMM::getResultado()
{
  return this->resultado;
}

float ResultadoHCMM::getCosto()
{
  return this->costo;
}

unsigned long ResultadoHCMM::getCobertura()
{
  return this->cobertura;
}

/**
 * Se hace una redirección entre descriptores de archivo. Se usa el nombre del archivo
 * para obtener un descriptor de dicho archivo. Luego el descriptor original (entregado
 * en la variable descriptor), es reemplazado por el descriptor de archivo mencionado
 * anteriormente. ¿Para que se usa?, se puede usar para reemplazar el descriptor
 * STDIN por el descriptor de archivo obtenido por nombre_archivo, y de esta
 * manera usar cin para leer el contenido del archivo, en lugar de hacer las operaciones
 * que se realizan al utilizar open o fopen.
 */
int redirigir_descriptor(const int descriptor, const char *nombre_archivo, const int modo)
{
  int descriptor_copiado, descriptor_nuevo;

  if ((descriptor_copiado = dup(descriptor)) == -1 ||
      (descriptor_nuevo = open(nombre_archivo, modo)) == -1 ||
      dup2(descriptor_nuevo, descriptor) == -1 ||
      close(descriptor_nuevo) == -1)
  {
    perror("Error: ");
    exit(-1);
  }

  /**
   * Este descriptor contiene el contenido del descriptor original (variable descriptor).
   * Usado para restaurar el valor del descriptor original.
  */
  return descriptor_copiado;
}

/**
 * Restaura el descriptor de archivo original.
 */
void restaurar_descriptor(const int descriptor_copiado, const int descriptor_modificado)
{
  /*redirigir el descriptor de entrada para que reciba la entrada estandar*/
  if (dup2(descriptor_copiado, descriptor_modificado) == -1 || close(descriptor_copiado) == -1)
  {
    perror("Error: ");
    exit(-1);
  }
}

std::tuple<float, unsigned int, unsigned int, unsigned int, const char *, posicion, posicion, posicion> cargar_datos(const char *nombre_archivo)
{
  float presupuesto;
  unsigned int n_eventos, radio, n_aeds_iniciales;
  unsigned long coord_x, coord_y, aed_inicial;
  posicion coords_x, coords_y, aeds_iniciales;
  int descriptor_original;

  descriptor_original = redirigir_descriptor(STDIN_FILENO, nombre_archivo, O_RDONLY);

  std::cin >> n_eventos >> presupuesto >> radio;
  for (unsigned int c = 0; c < n_eventos; c++)
  {
    std::cin >> coord_x >> coord_y >> aed_inicial;
    coords_x.push_back(coord_x), coords_y.push_back(coord_y), aeds_iniciales.push_back(aed_inicial);
    n_aeds_iniciales += aed_inicial;
  }

  restaurar_descriptor(descriptor_original, STDIN_FILENO);

  const char *enfoque = n_aeds_iniciales ? "flexible" : "fijo";

  return std::make_tuple(presupuesto, radio, n_eventos, n_aeds_iniciales,
                         enfoque, coords_x, coords_y, aeds_iniciales);
}

void establecer_semilla(long semilla)
{
  srand(semilla);
}

posicion generar_si_fijo(const int largo, unsigned long presupuesto, unsigned int n_aeds)
{
  posicion solucion(largo, 0);
  /*
   * Se genera una solución inicial con una cantidad de AEDs que varia desde presupuesto hasta
   * presupuesto + 40%, esto con el fin de hacer variar la cantidad de AEDs de cada solución
   * inicial, además se ha elegido que la cantidad de AEDs iniciales varie en esos rangos,
   * ya que en general la cantidad de AEDs que poseen las soluciones factibles están bastante cerca de
   * la cantidad de presupuesto con el cual se cuenta.
   */
  presupuesto = presupuesto * (1 + ((rand() % 4) / 10));
  for (auto iter = solucion.begin(); iter != solucion.end() && presupuesto > 0; iter++)
  {
    /*Se genera un 0 o 1 de forma aleatoria, que indica si existe o no un aed.*/
    unsigned long hay_aed = ((rand() % 101) / 100) >= 0.9;
    if (hay_aed)
    {
      *iter = 1;
      presupuesto--;
    }
  }
  return solucion;
}

posicion generar_si_flexible(const int largo, unsigned long presupuesto, unsigned int n_aeds)
{
  posicion solucion(largo, 0);
  /*
   * Se genera una solución inicial con la cantidad de aeds que se encuentran ubicados inicialmente
   * en las ubicaciones de OHCA dadas. Debido a que existe una cantidad inicial de AEDs que se
   * deben colocar en la solución, se decide aumentar la probabilidad de generar 1's.
   */
  for (auto iter = solucion.begin(); iter != solucion.end() && n_aeds > 0; iter++)
  {
    unsigned long hay_aed = ((rand() % 101) / 100) >= 0.7;
    if (hay_aed)
    {
      *iter = 1;
      n_aeds--;
    }
  }
  return solucion;
}
/*Calcula la cobetura total de la solución inicial*/
ResultadoCT cobertura_total_inicial(std::vector<std::set<coordenadas>> coberturas, posicion solucion_candidata)
{
  std::set<coordenadas> eventos_cubiertos;
  auto iter_cob = coberturas.begin();
  auto iter_sol = solucion_candidata.begin();
  for (; iter_cob != coberturas.end() && iter_sol != solucion_candidata.end(); iter_cob++, iter_sol++)
  {
    /*
     * Cada vez que un AED se encuentra cubriendo algún evento OHCA, se obtienen todas las posiciones
     * de eventos OHCA que se encuentran a una distancia radio de él, que a su vez serán posiciones
     * cubiertas por el AED.
     */
    if (*iter_sol)
    {
      eventos_cubiertos.insert((*iter_cob).begin(), (*iter_cob).end());
    }
  }
  return ResultadoCT(eventos_cubiertos.size(), eventos_cubiertos);
}

ResultadoCT cobertura_total(const std::vector<std::set<coordenadas>> coberturas,
                            const std::set<coordenadas> eventos_cubiertos, const posicion solucion_candidata,
                            const unsigned int posicion, const bool agregado)
{
  std::set<coordenadas> nuevos_eventos_cubiertos = eventos_cubiertos;
  /*
   * Debido a que el movimiento consiste en quitar o agregar un solo AED a la vez, entonces
   * cuando se agrega un AED en una posición se agrega a la cobertura de OHCA
   * actual los eventos OHCA cubiertos por el AED agregado y en caso de quitar un AED,
   * se hace diferencia simetrica entre la cobertura de OHCA actual y los
   * eventos OHCA cubiertos por el AED quitado.
   */

  if (!agregado)
  {
    std::set<coordenadas> nuevo;
    for (unsigned int count; count < solucion_candidata.size(); count++)
    {
      if (solucion_candidata[count])
      {
        nuevo.insert(coberturas[count].begin(), coberturas[count].end());
      }
    }
    return ResultadoCT(nuevo.size(), nuevo);
  }

  nuevos_eventos_cubiertos.insert(coberturas[posicion].begin(), coberturas[posicion].end());
  return ResultadoCT(nuevos_eventos_cubiertos.size(), nuevos_eventos_cubiertos);
}

float ccc_fijo(const posicion aeds_iniciales, const posicion solucion_candidata)
{
  return accumulate(solucion_candidata.begin(), solucion_candidata.end(), 0);
}

float ccc_flexible(const posicion aeds_iniciales, const posicion solucion_candidata)
{
  long aeds_movidos = 0;
  long n_aeds_iniciales = accumulate(aeds_iniciales.begin(), aeds_iniciales.end(), 0);
  long n_aeds_sol_candidata = accumulate(solucion_candidata.begin(), solucion_candidata.end(), 0);
  long aeds_agregados = n_aeds_sol_candidata - n_aeds_iniciales;
  aeds_agregados = aeds_agregados > 0 ? aeds_agregados : 0;
  for (auto iter_ini = aeds_iniciales.begin(), iter_sol = solucion_candidata.begin();
       iter_ini != aeds_iniciales.end() && iter_sol != solucion_candidata.end(); iter_ini++, iter_sol++)
  {
    /*
     * Si un aed estaba inicialmente en una ubicación pero en la solución candidata no aparece
     * en dicha posición, entonces el aed se ha movido.
     */
    if ((*iter_ini) == 1 && (*iter_sol) == 0)
    {
      aeds_movidos++;
    }
  }

  return aeds_agregados + (0.2 * aeds_movidos);
}

ResultadoFEv f_ev(const std::vector<std::set<coordenadas>> coberturas, const posicion aeds_iniciales,
                  const posicion solucion_candidata, const std::set<coordenadas> eventos_cubiertos,
                  const unsigned int posicion, const bool agregado, costo const cost, const unsigned long presupuesto)
{
  float costo = cost(aeds_iniciales, solucion_candidata);
  ResultadoCT cobertura = cobertura_total(coberturas, eventos_cubiertos, solucion_candidata, posicion,
                                          agregado);
  return ResultadoFEv(costo, cobertura.getCobertura(), cobertura.getEventosCubiertos());
}

std::vector<std::set<coordenadas>> obtener_coberturas(const std::vector<unsigned long> coords_x,
                                                      const std::vector<unsigned long> coords_y, const unsigned int radio)
{
  std::vector<std::set<coordenadas>> coberturas;
  /*
   * Se genera un vector, donde cada posición representa una determinada ubicación de un
   * evento OHCA y en cada una de esas posiciones se almacena el conjunto de todas aquellas
   * ubicaciones de eventos OHCAs que se encuentran a una distancia radio de él. De esta
   * manera, es posible almacenar la cobertura de un punto de OHCA y realizando uniones
   * entre los conjuntos de OHCA de cada posición del vector se puede obtener la cobertura
   * total de las soluciones entregadas por HC con MM.
   */
  for (auto iter_x_act = coords_x.begin(), iter_y_act = coords_y.begin();
       iter_x_act != coords_x.end() && iter_y_act != coords_y.end(); iter_x_act++, iter_y_act++)
  {

    std::set<coordenadas> cobertura;
    for (auto iter_x_sig = coords_x.begin(), iter_y_sig = coords_y.begin();
         iter_x_sig != coords_x.end() && iter_y_sig != coords_y.end(); iter_x_sig++, iter_y_sig++)
    {

      unsigned int radio_act = sqrt(pow(*iter_x_act - *iter_x_sig, 2) + pow(*iter_y_act - *iter_y_sig, 2));
      if (radio_act <= radio)
      {
        cobertura.insert(std::make_pair(*iter_x_sig, *iter_y_sig));
      }
    }
    coberturas.push_back(cobertura);
  }
  return coberturas;
}

void info_aeds_fijo(const posicion aeds_iniciales, const posicion mejor_solucion,
                    const posicion coords_x, const posicion coords_y)
{
  long n_aeds_agregados = accumulate(mejor_solucion.begin(), mejor_solucion.end(), 0);
  std::cout << "Cantidad de AEDs agregados: " << n_aeds_agregados << std::endl;
  std::cout << "Posiciones de los AEDs: " << std::endl;
  for (unsigned int pos = 0; pos < mejor_solucion.size(); pos++)
  {
    if (mejor_solucion[pos])
    {
      std::cout << "coordenada X: " << coords_x[pos] << " coordenada Y: " << coords_y[pos] << std::endl;
    }
  }
}

void info_aeds_flexible(const posicion aeds_iniciales, const posicion mejor_solucion,
                        const posicion coords_x, const posicion coords_y)
{
  long n_aeds_mejor_solucion = accumulate(mejor_solucion.begin(), mejor_solucion.end(), 0);
  long n_aeds_iniciales = accumulate(aeds_iniciales.begin(), aeds_iniciales.end(), 0);
  long n_aeds_agregados = n_aeds_mejor_solucion - n_aeds_iniciales;
  n_aeds_agregados = n_aeds_agregados > 0 ? n_aeds_agregados : 0;
  long n_aeds_movidos = 0;
  for (unsigned int pos = 0; pos < aeds_iniciales.size(); pos++)
  {
    if (aeds_iniciales[pos] == 1 && mejor_solucion[pos] == 0)
    {
      n_aeds_movidos++;
    }
  }
  std::cout << "Cantidad de AEDs agregados: " << n_aeds_agregados << std::endl;
  std::cout << "Cantidad de AEDs movidos: " << n_aeds_movidos << std::endl;
  for (unsigned int pos = 0; pos < mejor_solucion.size(); pos++)
  {
    if (mejor_solucion[pos])
    {
      std::cout << "coordenada X: " << coords_x[pos] << " coordenada Y: " << coords_y[pos] << std::endl;
    }
  }
}

ResultadoHCMM hc_mm(std::vector<std::set<coordenadas>> coberturas, posicion aeds_iniciales,
                    posicion solucion_inicial, const int radio, const float presupuesto, costo cost)
{
  /*
   * Variables terminadas en _act sirven para procesar datos de la mejor solución (actual)
   * que se utilizan para generar los vecinos mediante el movimiento elegido y obtener
   * sus coberturas.
   * Variables mejor_solucion que no terminan en _act y variables mejor se utilizan para
   * guardar la información asociada a la mejor solución encontrada al realizar la explotación.
   * Variable solucion_candidata representa un vecino.
   */
  float mejor_costo = cost(aeds_iniciales, solucion_inicial);
  float mejor_costo_act = mejor_costo;
  ResultadoCT resultado_cobertura = cobertura_total_inicial(coberturas, solucion_inicial);
  unsigned long mejor_cobertura = resultado_cobertura.getCobertura();
  unsigned long mejor_cobertura_act = mejor_cobertura;
  std::set<coordenadas> mejor_eventos_cubiertos_act = resultado_cobertura.getEventosCubiertos();
  std::set<coordenadas> mejor_eventos_cubiertos = mejor_eventos_cubiertos_act;
  posicion mejor_solucion = solucion_inicial;
  posicion solucion_candidata = mejor_solucion;
  bool aed_agregado;
  bool hay_mejor_solucion;
  do
  {
    /*Al inicio del proceso no hay una mejor solución*/
    hay_mejor_solucion = false;
    for (unsigned int count = 0; count < mejor_solucion.size(); count++)
    {
      /*
       * el movimiento consiste en quitar o agregar un aed en una determinada posición de un
       * evento OHCA.
       */
      solucion_candidata[count] = solucion_candidata[count] ? 0 : 1;
      aed_agregado = solucion_candidata[count];
      ResultadoFEv calidad = f_ev(coberturas, aeds_iniciales, solucion_candidata,
                                  mejor_eventos_cubiertos_act, count, aed_agregado, cost, presupuesto);
      /*
       * Se van a aceptar soluciones no factibles en caso de que el costo de la mejor solución
       * sea mayor al presupuesto, en caso contrario, solo se encontrarán soluciones factibles.
       * Por lo tanto se tienen los siguientes dos casos para considerar una solución como mejor:
       *
       * 1. Si el costo de la mejor solución es mayor al presupuesto y el costo de la nueva solución
       * es menor al costo de la mejor solución entonces se considera la nueva solución como
       * mejor solución (en este caso se aceptan soluciones factibles y no factibles, se busca bajar
       * el costo de las soluciones para que calcen con el presupuesto).
       *
       * 2. Si el costo de la nueva solución es menor o igual al presupuesto y tiene mejor cobertura
       * que la mejor solución entonces se vuelve mejor solución (solo se aceptan soluciones factibles,
       * se busca aumentar la cobertura manteniendo un costo dentro del presupuesto).
       */
      float costo_act = calidad.getCosto();
      unsigned long cobertura_act = calidad.getCobertura();
      if (((mejor_costo_act > presupuesto) && (costo_act < mejor_costo_act)) ||
          ((costo_act <= presupuesto) && (cobertura_act > mejor_cobertura_act)))
      {
        mejor_solucion = solucion_candidata;
        mejor_costo = costo_act;
        /*
         * Cuando la solución encontrada salga del presupuesto, entonces dicha solución no tendrá
         * cobertura, ya que es una solución no factible, en caso contrario se calcula su cobertura
         * total.
         */
        mejor_cobertura = mejor_costo > presupuesto ? 0 : cobertura_act;
        mejor_eventos_cubiertos = calidad.getEventosCubiertos();
        hay_mejor_solucion = true;
      }
      /*
       * Se aplica nuevamente el movimiento, para que el vector vuelva a su estado original.
       */
      solucion_candidata[count] = solucion_candidata[count] ? 0 : 1;
    }
    mejor_costo_act = mejor_costo;
    mejor_cobertura_act = mejor_cobertura;
    solucion_candidata = mejor_solucion;
    mejor_eventos_cubiertos_act = mejor_eventos_cubiertos;
  } while (hay_mejor_solucion);

  return ResultadoHCMM(mejor_solucion, mejor_costo, mejor_cobertura);
}

void resolver(const int n_eventos, const int radio, const float presupuesto,
              const int n_restart, const unsigned int n_aeds_iniciales,
              const posicion aeds_iniciales, const posicion coords_x,
              const posicion coords_y, costo const cost,
              generar_solucion const solucion, imprimir const info)
{

  ResultadoHCMM mejor;
  std::vector<std::set<coordenadas>> coberturas = obtener_coberturas(coords_x, coords_y, radio);
  for (int c = 0; c < n_restart; c++)
  {
    posicion solucion_inicial = solucion(n_eventos, presupuesto, n_aeds_iniciales);
    ResultadoHCMM nuevo = hc_mm(coberturas, aeds_iniciales, solucion_inicial, radio, presupuesto, cost);
    unsigned long mejor_cobertura = mejor.getCobertura(), nuevo_cobertura = nuevo.getCobertura();
    float mejor_costo = mejor.getCosto(), nuevo_costo = nuevo.getCosto();
    if ((nuevo_cobertura > mejor_cobertura) || ((nuevo_cobertura == mejor_cobertura) && (nuevo_costo < mejor_costo)))
    {
      mejor = nuevo;
    }
  }
  std::cout << "Cobertura: " << mejor.getCobertura() << std::endl;
  std::cout << "Porcentaje de cobertura: " << std::fixed << std::setprecision(2) << (mejor.getCobertura() / (float)n_eventos) * 100 << "%" << std::endl;
  std::cout << "Presupuesto sobrante: " << presupuesto - mejor.getCosto() << std::endl;
  info(aeds_iniciales, mejor.getResultado(), coords_x, coords_y);
}
