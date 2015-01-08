# Digitação de notas da OBF

[English Version](README.en.md)

Até o ano de 2007, a Olimpíada Brasileira de Física (OBF) disponibilizava às escolas que sediavam sua primeira fase um software de digitação *offline* de notas, como alternativa à digitação *online* em um sistema Web. A digitação *offline* era muito importante em localidades do Brasil que não dispunham de conexão estável à Internet.

Este repositório contém o código fonte completo do software cliente utilizado na edição de 2007 da OBF. Esse foi o primeiro ano em que foi utilizado um software multiplataforma (Linux, BSD e Windows), escrito em C++. Nos anos anteriores, era utilizado um software em Delphi que funcionava exclusivamente em Windows. O código do servidor (escrito em PHP) infelizmente foi perdido.

O propósito deste repositório é puramente arqueológico, já que este código está completamente aposentado. Nosso intuito é preservar esta parte da história da OBF. Infelizmente, naquela época ainda não tínhamos o costume de usar controle de versão para projetos pequenos, então todo o código foi subido em um único commit.

## Características do sistema

O sistema, apesar da pouca experiência do desenvolvedor e de ter sido escrito às pressas, em meio a um semestre lotado da graduação, possui algumas características interessantes:

* O arquivo gerado para envio à organização da OBF era assinado com HMAC. O intuito era fornecer uma pequena proteção (pequena pois a chave não era muito forte) contra adulteração das notas durante o trajeto do arquivo em mídia física.
* Esse arquivo podia ser enviado ao servidor por qualquer pessoa. Não era necessário direcionar um envelope necessariamente à organização da OBF. O responsável pela prova em determinada localidade com conexão precária à Internet poderia confiar o arquivo a uma pessoa que disposse de conexão estável. Bastava entrar em uma página pelo navegador e subir o arquivo.
* Era possível enviar os dados para o servidor a partir do próprio software cliente. Algumas pessoas preferiam digitar as notas no cliente em vez de digitar no site, apesar de possuírem conexão estável.
* Sempre que as notas eram enviadas, o servidor emitia um comprovante de envio assinado com HMAC, e com um QRCode para facilitar uma eventual leitura desse comprovante. Além de os dados serem salvos no banco, ficava tudo registrado em arquivos (inclusive dados anteriores que houvessem sido removidos pelo usuário) para permitir recuperação caso necessário.
