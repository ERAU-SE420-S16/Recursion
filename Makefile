all: clean
	$(MAKE) -C ./libs/ all
	gcc -lm -lcrypt -lrt -o encryptionTest encryptionTest.c ./libs/libSecureEncryption.a
	gcc -lm -lcrypt -lrt -o filelistGenTest filelistGenTest.c ./libs/libSecureEncryption.a
	gcc -lm -lcrypt -o loginTest loginTest.c ./libs/libSecureEncryption.a
	gcc -lm -lcrypt -lrt -o recursiveFileCryptTest recursiveFileCryptTest.c ./libs/libSecureEncryption.a

clean:
	rm -f *.o filelistGenTest loginTest
