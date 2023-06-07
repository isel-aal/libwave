Esta biblioteca proporciona um conjunto de funções
para manipulação de ficheiros de audio codificados em format WAVE.
Designadamente, interpretação e criação de ficheiros neste formato.

## Dependências

A ***wavelib*** utiliza a biblioteca Glib-2.0
```
$ sudo apt install libglib2.0-dev
```

## Gerar

Para gerar a biblioteca deve-se descarregar as fontes e executar make.
```
$ git clone https://github.com/isel-aal/libwave.git
$ cd libwave
$ make
```

## Instalar

A biblioteca pode ser instalada com o *script* ``install.sh``.
Este *script* recebe como argumento a diretoria de instalação. Em caso de omissão instala em ``/usr/local``.
```
$ sudo ./install.sh
```
Para que a biblioteca seja encontrada pelo *loader*,
a variável de ambiente ``LD_LIBRARY_PATH`` deve ser definida com o caminho para a diretoria
onde os ficheiros ``libwave.a`` e ``libwave.so`` foram instalados.
```
$ export LD_LIBRARY_PATH=/usr/local/lib
```
ou esse caminho deve ser acrescentado à *cache* do *loader*.
```
$ sudo ldconfig /usr/local/lib
```

## Utilizar

Sob a diretoria ``samples`` encontra-se um conjunto de exemplos de utilização da biblioteca.
Estes exemplos contêm um ficheiro ``makefile`` onde se invoca o utilitário ``pkg-config``.

Se a biblioteca ***wavelib*** não for instalada nas diretorias convencionais,
é necessário informar o utilitário ``pkg-config`` do local de instalação
do ficheiro ``libwave.pc``, através da variável de ambiente PKG_CONFIG_PATH.

```
$ export PKG_CONFIG_PATH=/usr/local/pkgconfig
```
