--TEST--
Tests Argon2 exceptions
--FILE--
<?php
try {
    $hash = argon2_hash('test', HASH_ARGON2, ['m_cost' => 0]);
} catch (InvalidArgumentException $e) {
    var_dump($e->getMessage());
}

try {
    $hash = argon2_hash('test', HASH_ARGON2, ['t_cost' => 0]);
} catch (InvalidArgumentException $e) {
    var_dump($e->getMessage());
}

try {
    $hash = argon2_hash('test', HASH_ARGON2, ['threads' => 0]);
} catch (InvalidArgumentException $e) {
    var_dump($e->getMessage());
}
--EXPECT--
string(24) "Memory cost is not valid"
string(22) "Time cost is not valid"
string(30) "Number of threads is not valid"