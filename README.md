# PHP Argon2 Extension
This PHP7 extension provides a simplified interface to the [Argon2](https://github.com/P-H-C/phc-winner-argon2) algorithm, the winner of the [Password Hashing Competition](https://password-hashing.net/). Argon2 is considered the successor to bcrypt/scrypt/pbkdf methods of securely hasing passwords. This project is in no way associated with or endorsed by the PHC team.

> Note this is extension is only compatible with PHP7+. Support for lower versions of PHP will not be considered.

## Design

## Compilation

```
git clone https://github.com/charlesportwoodii/php-argon2-ext
cd php-argon2-ext
phpize
./configure
make
make install

# Load the extension to your php.ini/php conf.d
# echo "extension=argon2.so" > /path/to/php.ini
```

## Usage

This extension provides the following methods for generating and verifying hashes.

```
argon2_hash();
```

```
argon2_verify();
```

## Frequently Asked Questions

## License

BSD-3-Clause. See [LICENSE.md](LICENSE.md) for more details.
