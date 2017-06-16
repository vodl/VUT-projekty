<?php

use Nette\Application\UI\Form;
use Nette\Mail\Message;

/*
 * Trida pro export vykazu pro pojistovny
 */

class ExportPresenter extends BasePresenter {

    /** @var Repository */
    private $prodejs;
    private $lekRepository;
    private $pojistovnaRep;
    private $obsahovalRep;

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
        $this['exportForm']['save']->caption = 'Odeslat';
    }

    public function renderExported($mailArray) {
        $mailArray = $_GET; //magie
        $this->template->arr = $mailArray;
        $this->template->prodejRep = $this->prodejs;
        $this->template->lekRep = $this->lekRepository;
    }

    /*
     * Tovarna pro formular
     */
    protected function createComponentExportForm() {
        $pojPairs = $this->pojistovnaRep->findAll()->fetchPairs('cislo', 'nazev');
        $pojPairs['0'] = 'Všem pojišťovnám';

        $form = new Form;


        $form->addSelect('pojistovna_cislo', 'Pojišťovna:', $pojPairs)
                ->setDefaultValue(0);

        $form->addText('ccMail', 'Kopie:')
                ->addRule(Form::EMAIL, 'Zadejte platnou emailovou adresu');

        $form->addSubmit('save', 'Save')
                        ->setAttribute('class', 'default')
                ->onClick[] = $this->exportFormSubmitted;

        $form->addSubmit('cancel', 'Storno')
                        ->setValidationScope(NULL)
                ->onClick[] = $this->formCancelled;

        $form->addProtection();
        return $form;
    }

    /**
     * Odesle mail podle sablony
     * @param type $arr pole s prodeji odkazujicimi na prodane leky na predpis
     * @param type $destMail mail pojistovny
     * @param type $ccMail mail kopie
     */
    public function sendMail($arr, $destMail, $ccMail) {

        if ("Windows" === PHP_OS) {
            $mailTemStr = '\..\templates\Emails\export.latte';
        } else {
            $mailTemStr = DIRECTORY_SEPARATOR . '..' . DIRECTORY_SEPARATOR . 'templates' . DIRECTORY_SEPARATOR . 'Emails' . DIRECTORY_SEPARATOR . 'email.latte';
        }

        $template = $this->createTemplate();
        $template->setFile(__DIR__ . $mailTemStr);
        $template->arr = $arr;
        $template->prodejRep = $this->prodejs;
        $template->lekRep = $this->lekRepository;


        $mail = new Message;
        $mail->setFrom('NOREPLY@IISLekarna.cz')
                //->addTo('vodl@seznam.cz')
                ->addTo($destMail)
                ->addCc($ccMail)
                ->setSubject('Výkaz od IIS lékárny')
                ->setHtmlBody($template)
                ->send();
 
 

        $this->flashMessage('email odeslán.');
    }

    //fotmular odeslan
    public function exportFormSubmitted($button) {
        $values = $button->getForm()->getValues();
        $pojcislo = $values['pojistovna_cislo'];
        $ccMail = $values['ccMail'];

        if ($pojcislo == 0) {
            $mysel = $this->prodejs->findAll()->where('pojistovna_cislo > ?', 0)->where('exportovano', NULL);
        } else {
            $mysel = $this->prodejs->findAll()->where('pojistovna_cislo', $pojcislo)->where('exportovano', NULL);
        }

        $mailArray = array();


        foreach ($mysel as $prodej) {

            if (!isset($mailArray[$prodej->pojistovna_cislo])) {
                $mailArray[$prodej->pojistovna_cislo] = array();
            }

            if (!isset($mailArray[$prodej->pojistovna_cislo[$prodej->id]])) {
                $mailArray[$prodej->pojistovna_cislo][$prodej->id] = array();
            }

            //prodeje, ktere byli na predpis
            $lekSel = $this->obsahovalRep->findall()->where('prodej_id', $prodej->id)->where('predepsany > ?', 0);
            foreach ($lekSel as $actLek) {
                array_push($mailArray[$prodej->pojistovna_cislo][$prodej->id], $actLek->lek_kod);
            }
        }

        //predepsane leky priradit prodejum pro pojistovny
        foreach ($mailArray as $actPoj) {
            $this->sendMail($actPoj, $this->pojistovnaRep->findById(key($mailArray))->email, $ccMail);
            foreach ($actPoj as $actProd) {
                $this->prodejs->findById(key($actPoj))->update(array('exportovano' => new \DateTime()));
                //oznacit exportovane prodeje
            }
        }
        $this->redirect('exported', array('myArr' => $mailArray));
    }

    public function formCancelled() {
        $this->redirect('homepage:default');
    }

}
