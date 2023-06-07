Esta biblioteca proporciona um conjunto de funçõespara manipulação de ficheiros de audio codificados em format WAVE.
Designadamente, interpretação e criação de ficheiros neste formato.

## Dependências

A ***wavelib*** utiliza a biblioteca Glib-2.0
```
$ sudo apt install libglib2.0-dev
```

## Gerar

Para gerar a biblioteca deve-se descarregar as fontes e executar ``make``.
```
$ git clone https://github.com/isel-aal/libwave.git
$ cd libwave
$ make
```

## Instalar

A biblioteca pode ser instalada com o *script* ``install.sh``.
Este *script* recebe como argumento a diretoria de instalação. Em caso de omissão instala em ``/usr/local``.
```
$ sudo ./install.sh ~/mydir
```
Para que a biblioteca seja encontrada pelo *loader*, o caminho para a diretoria onde os ficheiros ``libwave.a`` e ``libwave.so`` foram instalados deve ser acrescentado à *cache* do *loader*.
```
$ sudo ldconfig ~/mydir/lib
```
ou a variável de ambiente ``LD_LIBRARY_PATH`` deve ser definida com  esse caminho.
```
$ export LD_LIBRARY_PATH=~/mydir/lib
```

## Utilizar

Sob a diretoria ``samples`` encontra-se um conjunto de exemplos de utilização da biblioteca.
Estes exemplos contêm um ficheiro ``makefile`` onde se invoca o utilitário ``pkg-config``.

Se a biblioteca ***wavelib*** não for instalada nas diretorias convencionais, é necessário informar o utilitário ``pkg-config`` do local de instalação do ficheiro ``libwave.pc``, através da variável de ambiente PKG_CONFIG_PATH.

```
$ export PKG_CONFIG_PATH=~/mydir/lib/pkgconfig
```
Também é necessário corrigir a segunda linha do ficheiro ``~/mydir/lib/pkgconfig/wavelib.pc``.
Onde se lê ``prefix=/usr/local`` deve ler-se ``prefix=/home/<username>/mydir``.
(Aqui não se pode utilizar o ``~`` como equivalente à *home directory*.)