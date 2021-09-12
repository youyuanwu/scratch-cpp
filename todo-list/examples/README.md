# existing solutions for openapi

Install generator:
wget https://repo1.maven.org/maven2/org/openapitools/openapi-generator-cli/5.2.1/openapi-generator-cli-5.2.1.jar -O openapi-generator-cli.jar


Command:
```
java -jar openapi-generator-cli.jar generate \
   -i ../swagger.yml \
   -g cpp-qt-client \
   -o ./cpp-qt-client
```

* qt client
```
java -jar openapi-generator-cli.jar generate \
   -i ../swagger.yml \
   -g cpp-qt-client \
   -o ./cpp-qt-client
```

cpp-restsdk
```
java -jar openapi-generator-cli.jar generate \
   -i ../swagger.yml \
   -g cpp-restsdk \
   -o ./cpp-restsdk
```