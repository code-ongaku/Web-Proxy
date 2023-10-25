# ** FROG PROXY - USER MANUAL **
### The FROG PROXY is a simple proxy that filters content from the website http://pages.cpsc.ucalgary.ca/~jcleahy/. 

## Function:
It performs two main functions:
1. All instances of the word "frog" regardless of case would be changed to "Fred"
2. All jpg image files will be changed to display the image of a given ucalgary

## Compilation & Execution:

To use the FROG PROXY, please follow these steps:

1. Make sure your web browser is configured to use a proxy on your local machine. Set the proxy address to "127.0.0.1" (localhost) and specify the same port number as in the proxyFrog.cpp code.

**Note:** If binding the proxy fails, you may need to change both the port in the code and the proxy port in your browser settings.

2. Compile the code by navigating to the directory containing the proxyFrog.cpp file and executing the following command:

```bash
    g++ proxyFrog.cpp -o proxy
```

3. Run the proxy using the following command:

```bash
    ./proxy
```

4. Open your web browser and load the website http://pages.cpsc.ucalgary.ca/~jcleahy/. The proxy will apply the content filtering as described in the "Function" section.

## TESTING RESULTS 
Testing was conducted on the website http://pages.cpsc.ucalgary.ca/~jcleahy/ and for the individual pages linked on the homepage. The testing was performed both at home and at school.

During testing, the following results were observed:
1. The page "Frog Facts" had all instances of the word "frog" changed to "Fred" regardless of case.
2. The image in the page "Simple image" was replaced with a picture of "Frogue" from http://pages.cpsc.ucalgary.ca/~jcleahy/Frogue.jpg.
3. The images in the pages "Pond Listings" and "Listings" had all JPG image files replaced with "Frogue."
Note: PNG files in "Pond Listings" were not changed, as the proxy focuses on JPG files.
4. All instances of the word "frog" in the pages "Pond Listings" and "Listings" were changed to "Fred."

Note: The proxy filters only the whole word "frog" and does not affect substrings (e.g., "froggy," "frog-friendly," "frogs").

## ADDITIONAL
The code for the FROG PROXY includes commented print statements. These print statements were used for monitoring the proxy's requests and responses, as well as for debugging purposes during development.

The testing environment used for this project involved WSL (Windows Subsystem for Linux) to create a remote Linux environment for testing. The Mozilla Firefox browser was utilized for conducting the tests.