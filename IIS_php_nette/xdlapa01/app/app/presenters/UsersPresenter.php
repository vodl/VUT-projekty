<?php

use Nette\Application\UI\Form;



class UsersPresenter extends BasePresenter {

    /** @var LekRepository */
    private $userRep;
    private $usrid;
    private $aut;

    public function inject(UserRepository $userRep, Authenticator $aut) {
        $this->userRep = $userRep;
        $this->aut = $aut;
    }

    protected function startup() {
        parent::startup();

        if (!$this->user->isLoggedIn()) {
            if ($this->user->logoutReason === Nette\Http\UserStorage::INACTIVITY) {
                $this->flashMessage($this->sgoutStr);
            }
            $this->redirect('Sign:in', array('backlink' => $this->storeRequest()));
        }
        $usrRole = $this->userRep->findById($this->getUser()->getIdentity()->getId())->role;
        if ($usrRole !== 'admin') {
            $this->flashMessage('Nejste admin', 'err');
            $this->redirect('Sign:in', array('backlink' => $this->storeRequest()));
        }
    }

    public function renderDefault() {
        $this->template->usrs = $this->userRep->findAll();
    }


    public function renderAdd() {
        $this['usrForm']['save']->caption = 'Přidat';
    }

    public function renderEdit($id) {
        $form = $this['usrForm'];
        if (!$form->isSubmitted()) {
            $usr = $this->userRep->findById($id);
            if (!$usr) {
                $this->error('Record not found');
            }
            $this->usrid = $id;
            $form->setDefaults($usr);
        }
    }


    public function renderDelete($id = 0) {
        $this->template->usr = $this->userRep->findById($id);
        if (!$this->template->usr) {
            $this->error('Record not found');
        }
    }


    protected function createComponentUsrForm() {
        $form = new Form;

        $form->addText('username', 'Login:')
                ->setRequired('Vložte login.');

        $form->addText('name', 'Jméno:');
                //->setRequired('Zadejte název.');

        $form->addText('role', 'Role:')
                ->setRequired('Zadejte roli.');

        $form->addPassword('password', 'Heslo:')
                ->setRequired('Zvolte si heslo')
                ->addRule(Form::MIN_LENGTH, 'Heslo musí mít alespoň %d znaky', 3);

        $form->addPassword('passwordVerify', 'Heslo pro kontrolu:')
                ->setRequired('Zadejte heslo znovu.')
                ->addRule(Form::EQUAL, 'Hesla se neshodují', $form['password']);


        $form->addSubmit('save', 'Save')
                        ->setAttribute('class', 'default')
                ->onClick[] = $this->usrFormSubmitted;

        $form->addSubmit('cancel', 'Cancel')
                        ->setValidationScope(NULL)
                ->onClick[] = $this->formCancelled;

        $form->addProtection();
        return $form;
    }

    
    
    public function usrFormSubmitted($button) {
        $values = $button->getForm()->getValues();
        $myArr = array(
            'name' => $values['name'],
            'username' => $values['username'],
            'role' => $values['role'],
            'password' => $this->aut->generateHash($values['password'])
        );

        $id = (int) $this->getParameter('id');
        if ($id) {
            $this->userRep->findById($id)->update($myArr);
            $this->flashMessage('Uživatel pozměněn.');
        } else {
            $this->userRep->insert($myArr);
            $this->flashMessage('Uživatel přidán.');
        }
        $this->redirect('default');
    }


    
    protected function createComponentDeleteForm() {
        $form = new Form;
        $form->addSubmit('cancel', 'Storno')
                ->onClick[] = $this->formCancelled;

        $form->addSubmit('delete', 'Smazat')
                        ->setAttribute('class', 'default')
                ->onClick[] = $this->deleteFormSubmitted;

        $form->addProtection();
        return $form;
    }

    
    public function deleteFormSubmitted() {
        if ($this->user !== $this->getParameter('id')) {
            $this->userRep->findById($this->getParameter('id'))->delete();
            $this->flashMessage('Uživatel smazán.');
        } else {
            $this->flashMessage('Nelze smazat přihlášeného uživatele.');
        }
        $this->redirect('default');
    }

    
    public function formCancelled() {
        $this->redirect('default');
    }

}
