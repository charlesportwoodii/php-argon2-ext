# PHP Argon2 Extension
This PHP7 extension provides a simplified interface to the [Argon2](https://github.com/P-H-C/phc-winner-argon2) algorithm, the winner of the [Password Hashing Competition](https://password-hashing.net/). Argon2 is considered the successor to bcrypt/scrypt/pbkdf methods of securely hasing passwords. This project is in no way associated with or endorsed by the PHC team.

> Note this is extension is only compatible with PHP7+. Support for lower versions of PHP will not be considered.

## Design Goals

## Building

```bash
# Clone the extension and the Argon2 Submodule
git clone --recursive https://github.com/charlesportwoodii/php-argon2-ext
cd php-argon2-ext

# Build the Argon2 library
cd ext/argon2
make
cd ../..

# Build the extension
phpize
./configure
make
```

### Installation

Once you have compiled the extension, you can install it via `make install`, adding the extension to your `php.ini` file or to a file in your loaded extensions directory, 

```bash
$ make install
# Load the extension to your php.ini/php conf.d
# echo "extension=argon2.so" > /path/to/php.ini
```

### Testing

Extension is tested through `make test`. You are strongly encouraged to run the tests to make sure everything was built correctly. A summary of the tests will be outlined

```bash
$ make test
```

If `make test` encounters an error, please provide a copy of the error report as a Github issue.

## Usage

This extension provides the following methods for generating and verifying hashes.

```php
argon2_hash();
```

```php
argon2_verify();
```

## Reporting a Bug

## Frequently Asked Questions

## License

BSD-3-Clause. See [LICENSE.md](LICENSE.md) for more details.
