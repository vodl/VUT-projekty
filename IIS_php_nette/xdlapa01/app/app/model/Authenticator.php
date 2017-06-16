<?php
/** 
 * Podle prikladu CD-Collection, 
 * ktery je soucasti Open source frameworku Nette
 * 
 */

use Nette\Security,
	Nette\Utils\Strings;


/**
 * Autentifikace uzivatelu
 */
class Authenticator extends Nette\Object implements Security\IAuthenticator
{
	/** @var Nette\Database\Connection */
	private $database;



	public function __construct(Nette\Database\Connection $database)
	{
		$this->database = $database;
	}



	/**
	 * Provede authentifikaci.
	 * @param  array
	 * @return Nette\Security\Identity
	 * @throws Nette\Security\AuthenticationException
	 */
	public function authenticate(array $credentials)
	{
		list($username, $password) = $credentials;
		$row = $this->database->table('user')->where('username', $username)->fetch();

		if (!$row) {
			throw new Security\AuthenticationException('Špatné uživatelské jméno.', self::IDENTITY_NOT_FOUND);
		}

		if ($row->password !== $this->generateHash($password, $row->password)) {
			throw new Security\AuthenticationException('Nesprávné heslo.', self::INVALID_CREDENTIAL);
		}

		unset($row->password);
		return new Security\Identity($row->id, NULL, $row->toArray());
	}



	/**
	 * Výpočet hashe.
	 * @param  string
	 * @param  string
	 * @return string
	 */
	public function generateHash($password, $salt = NULL)
	{
		if ($password === Strings::upper($password)) {
			$password = Strings::lower($password);
		}
		return crypt($password, $salt ?: '$2a$07$' . Strings::random(23));
	}

}
