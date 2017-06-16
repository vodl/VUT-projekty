<?php

use Nette\Application\UI\Form;

class ProdejPresenter extends BasePresenter {

    /** @var Repository */
    private $prodejs;
    private $lekRepository;
    private $pojistovnaRep;
    private $obsahovalRep;
    public $actRow;

    /** @Persistent */
    public $actId;


    public function inject(ProdejRepository $prodej, LekRepository $lekRepository, PojistovnaRepository $pojistovnaRep, ObsahovalRepository $obsahovalRep) {
        $this->prodejs = $prodej;
        $this->lekRepository = $lekRepository;
        $this->pojistovnaRep = $pojistovnaRep;
        $this->obsahovalRep = $obsahovalRep;
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
        $this->template->prodejs = $this->prodejs->findAll();
    }
    
        public function renderDetail($id) {
        $this->template->leks = $this->obsahovalRep->findBy(array('prodej_id' => $id));
    }

    public function renderAdd() {
        $this['prodejForm']['save']->caption = 'Vytvořit';
    }


    public function actionNextlek($id) {
        $form = $this['nextlekForm'];
        $form->addHidden('prodej_id', $id); 
        $this->template->form = $form;
    }

    public function renderEdit($id = 0) {
        $form = $this['prodejForm'];
        if (!$form->isSubmitted()) {
            $prodej = $this->prodejs->findById($id);
            if (!$prodej) {
                $this->error('Nenalezeno');
            }
            $form->setDefaults($prodej);
        }
    }

    public function renderDelete($id = 0) {
        $this->template->prodej = $this->prodejs->findById($id);
        if (!$this->template->prodej) {
            $this->error('Prodej nenalezen');
        }
    }


    protected function createComponentProdejForm() {
        $pojPairs = $this->pojistovnaRep->findAll()->fetchPairs('cislo', 'nazev');

        $form = new Form;

        $form->addSelect('pojistovna_cislo', 'Pojišťovna:', $pojPairs)
                ->setDefaultValue(0);


        $form->addSubmit('save', 'Save')
                        ->setAttribute('class', 'default')
                ->onClick[] = $this->prodejFormSubmitted;

        $form->addSubmit('cancel', 'Cancel')
                        ->setValidationScope(NULL)
                ->onClick[] = $this->formCancelled;

        $form->addProtection();
        return $form;
    }


    protected function createComponentNextlekForm() {

        $lekPairs = $this->lekRepository->findAll()->fetchPairs('kod', 'nazev');

        $form = new Form;

        $form->addSelect('lek', 'Lék:', $lekPairs)
                ->setPrompt('- Vyberte -')
                ->addRule(Form::FILLED, 'Není zvolen lék');

        $form->addText('kusu', 'Počet kusů', 2)
                ->setValue('1')
                ->addRule(Form::INTEGER, 'Neplatná hodnota množství')
                ->addRule($form::RANGE, 'Neplatné množství', array(1, 99));

        $form->addCheckbox('predepsany', 'Předepsaný');

        $form->addSubmit('save', 'Přidat')
                        ->setAttribute('class', 'default')
                ->onClick[] = $this->nextlekFormSubmitted;

        $form->addSubmit('cancel', 'Hotovo')
                        ->setValidationScope(NULL)
                ->onClick[] = $this->formCancelled;

        $form->addProtection();
        return $form;
    }

    public function prodejFormSubmitted($button) {
        $values = $button->getForm()->getValues();
        $pojcislo = $values['pojistovna_cislo'];

        $this->actRow = $this->prodejs->createProdej($pojcislo);
        $this->flashMessage('Prodej vytvořen.');

        $this->actId = $this->actRow->getPrimary();

        $this->redirect('nextlek', $this->actRow->getPrimary());
    }

    
    
    public function nextlekFormSubmitted($button) {


        $values = $button->getForm()->getValues();
        $skladem = $this->lekRepository->findById($values['lek'])->skladem;

        if ($values['kusu'] > $skladem) {
            $this->flashMessage('Zvolené množství nebylo skladem!!!', 'err');
            $this->redirect('nextlek', $values["prodej_id"]);
        } else if ($this->lekRepository->findById($values['lek'])->na_predpis && !($values['predepsany'])) {
            $this->flashMessage('Není možné vložit lék na předpis do prodeje, který nemá uvedenou pojišťovnu!!', 'err');
            $this->redirect('nextlek', $values["prodej_id"]);
        } else {

            for ($i = 1; $i <= $values['kusu']; $i++) {
                $this->obsahovalRep->insert(
                        array('lek_kod' => $values["lek"],
                            'prodej_id' => $values["prodej_id"],
                            'predepsany' => $values['predepsany']
                ));
            }
            $this->lekRepository->findById($values['lek'])->update(array('skladem' => ($skladem - $values['kusu'])));
            $this->flashMessage('Položka přidána.');
            $this->redirect('nextlek', $values["prodej_id"]);
        }
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
        $this->prodejs->findById($this->getParameter('id'))->delete();
        $this->flashMessage('Prodej smazán.');
        $this->redirect('default');
    }
    
    

    public function formCancelled() {
        $this->redirect('default');
    }

}
