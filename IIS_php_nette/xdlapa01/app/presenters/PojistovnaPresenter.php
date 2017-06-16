<?php

use Nette\Application\UI\Form;

class PojistovnaPresenter extends BasePresenter {

    /** @var LekRepository */
    private $userRep;
    private $pojcislo;
    private $pojRep;
    public $pojCislo;

    public function inject(UserRepository $userRep, PojistovnaRepository $pojRep) {
        $this->userRep = $userRep;
        $this->pojRep = $pojRep;
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
        $this->template->pojs = $this->pojRep->findAll();
    }

    public function renderAdd() {
        $this['pojForm']['save']->caption = 'Přidat';
    }

    public function renderEdit($id) {
        $form = $this['pojForm'];
        if (!$form->isSubmitted()) {
            $poj = $this->pojRep->findById($id);
            if (!$poj) {
                $this->error('Nenalezeno');
            }
            $this->pojcislo = $id;
            $form->setDefaults($poj);
        }
    }

    /*
      public function renderDelete($id = 0) {
      $this->template->poj = $this->pojRep->findById($id);
      if (!$this->template->poj) {
      $this->error('Nenalezeno');
      }
      $this->pojCislo = $id;
      } */

    public function actionDelete($id) {
        $this->template->poj = $this->pojRep->findById($id);
        if (!$this->template->poj) {
            $this->error('Nenalezeno');
        }

        $form = $this['deleteForm']; //['save']->caption = 'Přidat';   //zavola metodu createComponentNazovformulara a vrati ti formular

        $form->addHidden('cislo', $id);  //pridas tam to idecko
        $this->template->form = $form;      //predas formular do sablony
    }

    /**
     * Tovarna na formular pro pojistovnu
     * @return Form
     */
    protected function createComponentPojForm() {
        $form = new Form;
        $form->addText('cislo', 'Číslo:')
                ->setRequired('Vložte číslo pojišťovny.');

        $form->addText('nazev', 'Název:');

        $form->addText('email', 'Email:')
                ->setRequired()
                ->addRule(Form::EMAIL, 'Zadejte platný email');



        $form->addSubmit('save', 'Save')
                        ->setAttribute('class', 'default')
                ->onClick[] = $this->pojFormSubmitted;

        $form->addSubmit('cancel', 'Cancel')
                        ->setValidationScope(NULL)
                ->onClick[] = $this->formCancelled;

        $form->addProtection();
        return $form;
    }

    public function pojFormSubmitted($button) {
        $values = $button->getForm()->getValues();

        $id = (int) $this->getParameter('cislo');
        if ($id) {
            $this->pojRep->findById($id)->update($values);
            $this->flashMessage('Údaje změněny.');
        } else {
            $this->pojRep->insert($values);
            $this->flashMessage('Pojišťovna přidána.');
        }
        $this->redirect('default');
    }

    protected function createComponentDeleteForm() {
        $form = new Form;
        $form->addSubmit('cancel', 'Storno')
                ->onClick[] = $this->formCancelled;

        $form->addSubmit('delete', 'Smazat')
                        ->setAttribute('class', 'default')
                ->onClick[] = callback($this, 'deleteFormSubmitted');

        $form->addProtection();
        return $form;
    }

    public function deleteFormSubmitted($button) {
        $values = $button->getForm()->getValues();
        $myId = $values['cislo'];


        if ($this->pojRep->findById($myId)->related('prodej')->fetch()) {
            $this->flashMessage('Není možné smazat pojišťovnu, která figuruje v prodeji', 'err');
        } else {
            $this->pojRep->findById($myId)->delete();
            $this->flashMessage('Smazáno');
        }
        $this->redirect('default');
    }

    public function formCancelled() {
        $this->redirect('default');
    }

}
