<?php

use Nette\Application\UI\Form;

class LekPresenter extends BasePresenter {

    /** @var LekRepository */
    private $leks;

    /** @persistent int */
    public $pid;

    public function inject(LekRepository $leks) {
        $this->leks = $leks;
    }

    protected function startup() {
        parent::startup();

        if (!$this->user->isLoggedIn()) {
            if ($this->user->logoutReason === Nette\Http\UserStorage::INACTIVITY) {
                $this->flashMessage($this->sgoutStr);
            }
            $this->redirect('Sign:in', array('backlink' => $this->storeRequest()));
        }
    }

    public function renderDefault() {
        $this->template->leks = $this->leks->findAll()->order('nazev');
    }

    public function renderFounded($id) {
        $this->template->lek = $this->leks->findById($id);
    }

    public function renderAdd() {
        $this['lekForm']['save']->caption = 'Přidat';
    }

    public function renderSearch() {
        $this['searchForm']['save']->caption = 'Hledat';
    }

    public function renderEdit($id) {
        $form = $this['lekeditForm'];
        if (!$form->isSubmitted()) {
            $lek = $this->leks->findById($id);
            if (!$lek) {
                $this->error('Record not found');
            }
            $this->pid = $id;
            $form->setDefaults($lek);
        }
    }

    public function renderDelete($id = 0) {
        $this->template->lek = $this->leks->findById($id);
        if (!$this->template->lek) {
            $this->error('Record not found');
        }
    }

    /*
     * Tovarna pro formular noveho leku
     */

    protected function createComponentLekForm() {
        $form = new Form;

        $form->addText('kod', 'Kód:')
                ->setRequired('Vložte kód.')
                ->addRule(Form::INTEGER, 'Kód musí být číslo');


        $form->addText('nazev', 'Název:')
                ->setRequired('Zadejte název.');

        $form->addText('cena', 'Cena:')
                ->setRequired('Zadejte cenu.')
                ->addRule(Form::INTEGER, 'Věk musí být číslo');

        $form->addCheckbox('na_predpis', 'Na předpis:');

        $form->addText('skladem', 'Skladem:')
                ->setRequired('Zadejte počet kusů skladem.')
                ->addRule(Form::INTEGER, 'Věk musí být číslo');

        $form->addText('doplatek', 'Doplatek:')
                ->addRule(Form::INTEGER, 'Doplatek musí být číslo')
                ->setDefaultValue('0');


        $form->addSubmit('save', 'Save')
                        ->setAttribute('class', 'default')
                ->onClick[] = $this->lekFormSubmitted;

        $form->addSubmit('cancel', 'Cancel')
                        ->setValidationScope(NULL)
                ->onClick[] = $this->formCancelled;

        $form->addProtection();
        return $form;
    }

    protected function createComponentSearchForm() {
        $form = new Form;

        $form->addText('kod', 'Kód:')
                ->setRequired('Vložte kód.');


        $form->addSubmit('save', 'Save')
                        ->setAttribute('class', 'default')
                ->onClick[] = $this->searchFormSubmitted;

        $form->addSubmit('cancel', 'Cancel')
                        ->setValidationScope(NULL)
                ->onClick[] = $this->formCancelled;

        $form->addProtection();
        return $form;
    }

    /**
     * Formular pro editaci leku
     * @return Form
     */
    protected function createComponentLekeditForm() {
        $form = new Form;

        $form->addText('nazev', 'Nazev:')
                ->setRequired('Zadejte nazev.');

        $form->addText('cena', 'Cena:')
                ->setRequired('Zadejte cenu.')
                ->addRule(Form::INTEGER, 'Cena musí být číslo');


        $form->addCheckbox('na_predpis', 'Na předpis:');

        $form->addText('skladem', 'Skladem:')
                ->setRequired('Zadejte počet kusů skladem.')
                ->addRule(Form::INTEGER, 'Cena musí být číslo');

        $form->addText('doplatek', 'Doplatek:')
                ->addRule(Form::INTEGER, 'Doplatek musí být číslo');



        $form->addSubmit('save', 'Save')
                        ->setAttribute('class', 'default')
                ->onClick[] = $this->lekeditFormSubmitted;

        $form->addSubmit('cancel', 'Cancel')
                        ->setValidationScope(NULL)
                ->onClick[] = $this->formCancelled;

        $form->addProtection();
        return $form;
    }

    public function lekFormSubmitted($button) {
        $values = $button->getForm()->getValues();
        $id = (int) $this->getParameter('kod');
        if ($id) {
            $this->leks->findById($id)->update($values);
            $this->flashMessage('Lék byl pozměněn.');
        } else {
            $this->leks->insert($values);
            $this->flashMessage('Lék byl přidán.');
        }
        $this->redirect('default');
    }

    public function searchFormSubmitted($button) {
        $values = $button->getForm()->getValues();
        $id = $values['kod'];
        if ($this->leks->findById($id)) {
            $this->flashMessage('Lék nalezen.');
            $this->redirect('founded', $id);
        } else {
            $this->flashMessage('Lék nenalezen', 'err');
            $this->redirect('default');
        }
    }

    public function lekeditFormSubmitted($button) {
        $values = $button->getForm()->getValues();

        $this->leks->findById($this->pid)->update($values);
        $this->flashMessage('Lék pozměněn.');

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
        $myId = $this->getParameter('id');
        if ($this->leks->findById($myId)->related('obsahoval')->fetch()) {
            $this->flashMessage('Není možné smazat lék, který figuruje v prodeji', 'err');
        } else {
            $this->leks->findById($myId)->delete();
            $this->flashMessage('Lék smazán.');
        }
        $this->redirect('default');
    }

    public function formCancelled() {
        $this->redirect('default');
    }

}
