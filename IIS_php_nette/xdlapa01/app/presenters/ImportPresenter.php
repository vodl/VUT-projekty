<?php

use Nette\Application\UI\Form;

class ImportPresenter extends BasePresenter {

    /** @var LekRepository */
    private $leks;

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
        
    }

    public function renderImport() {
        $this['importForm']['save']->caption = 'Nahrát';
    }

    protected function createComponentImportForm() {
        $form = new Form;

        $form->addUpload('file', 'Soubor:')
                //->addRule(Form::MIME_TYPE, 'Typ souboru musí být CSV', 'csv')
                ->addRule(Form::MAX_FILE_SIZE, 'Maximální velikost souboru je 5MB.', 5000 * 1024 /* v bytech */);


        $form->addSubmit('save', 'Nahrát')
                        ->setAttribute('class', 'default')
                ->onClick[] = callback($this, 'importFormSubmitted');

        $form->addSubmit('cancel', 'Storno')
                        ->setValidationScope(NULL)
                ->onClick[] = callback($this, 'formCancelled');

        $form->addProtection();
        return $form;
    }

    /*
     * Akce po odeslani formulare
     */
    public function importFormSubmitted($button) {

        $values = $button->getForm()->getValues();

        $frec = $values['file']->getTemporaryFile();
        if (TRUE) {// tato kontrola nebyla potreba
            if (($handle = fopen($frec, "r")) !== FALSE) {

                while (($data = fgetcsv($handle, 100, ",")) !== FALSE) {

                    if ($this->leks->findById($data[0]) && count($data) >= 2) {
                        $this->leks->findById($data[0])->update(array('doplatek' => $data[1]));
                    }
                }
                fclose($handle);
                $this->flashMessage("Import proveden!");
                $this->redirect('Lek:default');
            } else {
                $this->flashMessage("Chyba souboru!", "err");
                $this->redirect('default');
            }
        }
    }

    public function formCancelled() {
        $this->redirect('default');
    }

}