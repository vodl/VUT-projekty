<?php
/*
 * Vyuzit priklad, ktery je soucasti frameworku
 */

use Nette\Application\UI;


class SignPresenter extends BasePresenter
{
	/** @persistent */
	public $backlink = '';


	/**
	 * Tovarna pro formular.
	 * @return Nette\Application\UI\Form
	 */
	protected function createComponentSignInForm()
	{
		$form = new UI\Form;
		$form->addText('username', 'Login:')
			->setRequired('Zadejte login.');

		$form->addPassword('password', 'Heslo:')
			->setRequired('Zadejte heslo.');

		$form->addSubmit('send', 'Přihlásit');

		$form->onSuccess[] = $this->signInFormSubmitted;
		return $form;
	}



	public function signInFormSubmitted($form)
	{
		try {
			$values = $form->getValues();
			$this->getUser()->login($values->username, $values->password);
                        $this->getUser()->setExpiration('+ 5 minutes', TRUE);

		} catch (Nette\Security\AuthenticationException $e) {
			$form->addError($e->getMessage());
			return;
		}

		$this->restoreRequest($this->backlink);
		$this->redirect('Homepage:');
	}



	public function actionOut()
	{
		$this->getUser()->logout();
		$this->flashMessage('Odhlášení provedeno.');
		$this->redirect('in');
	}

}
